#include "segment_evaluator_mdfa.hpp"


#include <boost/container/flat_set.hpp>
#include "automata/dfa/dstate.hpp"
#include "util/timer.hpp"

#define FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

MacroSegmentEvaluator::MacroSegmentEvaluator(RegEx &rgx,
                                             std::shared_ptr<StrDocument> d,
                                             Anchor a, EvalStats &e)
    : rgx_(rgx), enumerator_(rgx_), text_(d), anchor_(a), stats_(e) {

  eva_ = std::make_unique<ExtendedVA>(rgx_.logicalVA(), anchor_);
  sva_ = std::make_unique<SearchVA>(rgx_.logicalVA(), anchor_);

  stats_.eva_size = eva_->size();
  stats_.sva_size = sva_->size();

  dfa_ = std::make_unique<DFA>(*eva_);
  sdfa_ = std::make_unique<SearchDFA>(*sva_);

  mdfa_ = std::make_unique<MacroDFA>(*dfa_);

  current_dstate_ = sdfa_->initial_state();

  bottom_node_ = ds_.bottom_node();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

Match_ptr MacroSegmentEvaluator::next() {

Enumeration:
  if (enumerator_.has_next())
    return enumerator_.next();

Evaluation:
  if (i_pos_ < i_max_) {
    if (i_pos_ < i_min_) // Then we know that the current runs are useless
      init_evaluation_phase(i_min_);
    if (evaluation_phase()) { // Then there's output to enumerate
      pass_outputs();
      goto Enumeration;
    }
  }

  // Searching
  init_searching_phase();
  if (searching_phase())
    goto Evaluation;

  stats_.dfa_size = dfa_->size();
  stats_.sdfa_size = sdfa_->size();
  stats_.mdfa_size = mdfa_->size();
  stats_.n_nodes = ds_.n_nodes();
  stats_.n_reused_nodes = ds_.n_reused_nodes();

  return nullptr;
}

void MacroSegmentEvaluator::init_evaluation_phase(int64_t pos) {
  dfa_->init_state()->set_node(bottom_node_);

  std::vector<DFA::State*> init_dstates;

  for (auto &elem : dfa_->init_eval_states()) {
    DFA::State *q0 = elem.first;
    std::bitset<32> S = elem.second;
    if (S != 0)
      visit_capture(dfa_->init_state(), S, q0, pos - 1);
    init_dstates.push_back(elem.first);
  }

  if (pos == 0)
    mdfa_->set_as_init(mdfa_->add_state(init_dstates));

  current_state_ = &mdfa_->get_init_state();

  i_pos_ = pos;
}

bool MacroSegmentEvaluator::evaluation_phase() {
  while (i_pos_ < i_max_) {
    char a = (*text_)[i_pos_];
    a &= 0x7F; // Only ASCII chars for now

    reading(a, i_pos_);

    ++i_pos_;

    if (!reached_final_states_.empty())
      return true; // On-line output
  }

  return false;
}

void MacroSegmentEvaluator::init_searching_phase() {}

bool MacroSegmentEvaluator::searching_phase() {

  i_min_ = i_src_;
  i_max_ = i_src_;

  for(;i_src_ < text_->size();++i_src_) {

    char a = (char)(*text_)[i_src_] & 0x7F;

    // nextState is reached from currentState by reading the character
    SDState *next_state = current_dstate_->next_state(a);

    if (next_state == nullptr) // Then maybe a determinization is needed
      current_dstate_ = sdfa_->next_state(current_dstate_, a);
    else
      current_dstate_ = next_state;

    if(current_dstate_->accepting())
      i_max_ = i_src_ + 1;
    else if(current_dstate_->ends()) {
      if(i_min_ < i_max_) {
        stats_.n_search_intervals++;
        stats_.search_intervals.emplace_back(std::make_pair(i_min_, i_max_));
        return true;
      }
      i_min_ = i_src_;
    }
  }

  if (i_min_ < i_max_) {
    stats_.n_search_intervals++;
    stats_.search_intervals.emplace_back(std::make_pair(i_min_, i_max_));
    return true;
  }

  i_min_ = text_->size();

  return false;
}

FORCE_INLINE void MacroSegmentEvaluator::reading(char a, int64_t pos) {

  auto nt = current_state_->next_transition(a);

  if (nt == nullptr)
    nt = mdfa_->next_transition(current_state_, a);


  boost::container::flat_set<ECS::Node*> decreasing_nodes;

  for (int i = 0; i < nt->nempties_; i++) {
    auto e = nt->empties()[i];
    e->node()->dec_ref_count();
    ds_.try_mark_unused(e->node());
  }

  for (int i = 0; i < nt->nfirstdirects_; i++) {
    auto d = nt->first_directs()[i];
    // decreasing_nodes.insert(d.from->node);
    d.to->set_node(d.from->node());
    if (d.to->accepting())
      reached_final_states_.push_back(d.to);
  }

  for (int i = 0; i < nt->nfirstcaptures_; i++) {
    auto c = nt->first_captures()[i];
    // decreasing_nodes.insert(c.from->node);
    auto nn = ds_.extend(c.from->node(), c.S, pos + 1);
    c.to->set_node(nn); // Slow pass node because c.from might be already visited
    if (c.to->accepting())
      reached_final_states_.push_back(c.to);
  }

  for (int i = 0; i < nt->nrepeatcaptures_; i++) {
    auto c = nt->repeat_captures()[i];

    decreasing_nodes.insert(c.to->node());
    decreasing_nodes.insert(c.from->node());

    c.to->set_node(
      ds_.unite(ds_.extend(c.from->node(), c.S, pos + 1), c.to->node()));
  }

  for (int i = 0; i < nt->nrepeatdirects_; i++) {
    auto d = nt->repeat_directs()[i];

    decreasing_nodes.insert(d.to->node());
    decreasing_nodes.insert(d.from->node());

    d.to->set_node(ds_.unite(d.from->node(), d.to->node()));
  }

  for(auto& node: decreasing_nodes) {
    node->dec_ref_count();
    ds_.try_mark_unused(node);
  }


  current_state_ = nt->next_state();
}

inline void MacroSegmentEvaluator::pass_outputs() {
  for (auto &state : reached_final_states_) {
    enumerator_.add_node(state->node());
    ds_.try_mark_unused(state->node());
    // state->node = nullptr;
    // NOTE: In MacroDFA we're not realising the accepting state from the next
    // iteration. In a normalized automaton, unanchored search, the accepting
    // state wont have any p
  }
  reached_final_states_.clear();
}

inline void MacroSegmentEvaluator::visit_capture(DFA::State *cs, std::bitset<32> S,
                                                 DFA::State *to, int64_t pos) {
  if (to->visited() <= pos) {
    to->set_node(ds_.extend(cs->node(), S, pos + 1));
    to->set_visited(pos + 1);
    if (to->accepting())
      reached_final_states_.push_back(to);
    else
      new_states_.push_back(to);
  } else {
    // Decrease the refcount, as the node at reached state won't be pointed by
    // that state anymore, only by the structure internally.
    to->node()->dec_ref_count();
    to->set_node(ds_.unite(ds_.extend(cs->node(), S, pos + 1), to->node()));
  }
}

} // end namespace rematch