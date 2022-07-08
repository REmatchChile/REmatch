#include "segment_evaluator.hpp"

#include "automata/dfa/dstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

SegmentEvaluator::SegmentEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d,
                                   Anchor a, EvalStats &e)
    : rgx_(rgx), enumerator_(rgx_), text_(d), anchor_(a), stats_(e) {

  eva_ = std::make_unique<ExtendedVA>(rgx_.logicalVA(), anchor_);
  sva_ = std::make_unique<SearchVA>(rgx_.logicalVA(), anchor_);

  // if(eva_->is_static()) {
  //   next_ = [this]() -> Match_ptr { return static_next(); };
  //   current_static_mapping_ = std::vector<int64_t>(rgx.vfactory()->size()*2, -1);
  //   for(size_t i = 0; i < rgx.vfactory()->size()*2; ++i)
  //     if (eva_->static_capture()[i])
  //       static_capture_positions_.push_back(i);
  // } else {
  //   next_ = [this]() -> Match_ptr { return normal_next(); };
  // }

  stats_.eva_size = eva_->size();
  stats_.sva_size = sva_->size();

  dfa_ = std::make_unique<DFA>(*eva_);
  sdfa_ = std::make_unique<SearchDFA>(*sva_);

  current_dstate_ = sdfa_->initial_state();

  bottom_node_ = ds_.bottom_node();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

Match_ptr SegmentEvaluator::next() {

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

  // for(auto s1: sdfa_->states) {
  //   for(auto s2: sdfa_->states) {
  //     // std::cout << "Checking " << s1->id() << " and " << s2->id() << '\n';
  //     if(s1->bitmap() == s2->bitmap())
  //       std::cerr << "ERROR: "  << s1->id() << " and " << s2->id() << "\n";
  //   }
  // }

  stats_.n_nodes = ds_.n_nodes();
  stats_.n_reused_nodes = ds_.n_reused_nodes();

  return nullptr;
}

void SegmentEvaluator::init_evaluation_phase(int64_t pos) {
  dfa_->init_state()->node = bottom_node_;

  current_states_.clear();

  for (auto &elem : dfa_->init_eval_states()) {
    DState *q0 = elem.first;
    std::bitset<32> S = elem.second;
    if (S != 0)
      visit(q0, ds_.extend(dfa_->init_state()->node, S, pos), pos - 1);
    current_states_.push_back(elem.first);
  }
  i_pos_ = pos;
}

bool SegmentEvaluator::evaluation_phase() {
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


void SegmentEvaluator::init_searching_phase() { }

void SegmentEvaluator::update_current_static_mapping(int64_t i) {
  for(int j: static_capture_positions_) {
    current_static_mapping_[j] = i - rgx_.vfactory()->get_offset(j);
  }
}

Match_ptr SegmentEvaluator::static_next() {
  for(;i_src_ < text_->size();++i_src_) {

    char a = (char) (*text_)[i_src_] & 0x7F;

    // nextState is reached from currentState by reading the character
    SDState* next_state = current_dstate_->next_state(a);

    if(next_state == nullptr) // Then maybe a determinization is needed
      current_dstate_ = sdfa_->next_state(current_dstate_, a);
    else
      current_dstate_ = next_state;

    if(current_dstate_->accepting()) {
      update_current_static_mapping(i_src_);
      ++i_src_;
      return std::make_unique<Match>(rgx_.vfactory(), current_static_mapping_);
    }
  }

  return nullptr;
}

bool SegmentEvaluator::searching_phase() {

  i_min_ = i_src_;
  i_max_ = i_src_;

  for (; i_src_ < text_->size(); ++i_src_) {

    char a = (char)(*text_)[i_src_] & 0x7F;

    // nextState is reached from currentState by reading the character
    SDState *next_state = current_dstate_->next_state(a);

    if (next_state == nullptr) // Then maybe a determinization is needed
      current_dstate_ = sdfa_->next_state(current_dstate_, a);
    else
      current_dstate_ = next_state;

    if (current_dstate_->accepting())
      i_max_ = i_src_ + 1;
    else if (current_dstate_->ends()) {
      if (i_min_ < i_max_) {
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

FORCE_INLINE void SegmentEvaluator::reading(char a, int64_t pos) {

  new_states_.clear();

  for (auto &p : current_states_) {
    auto nextTransition = p->next_transition(a);

    if (nextTransition == nullptr) {
      nextTransition = dfa_->next_transition(p, a);
    }

    auto *c = nextTransition->capture_;
    auto *d = nextTransition->direct_;

    internal::ECS::Node* from_node = p->node;

    if (nextTransition->type_ == Transition::Type::kDirect) {
      visit(d, from_node, pos);
    } else if (nextTransition->type_ == Transition::Type::kDirectSingleCapture) {
      visit(d, from_node, pos, false);
      from_node->inc_ref_count();
      visit(c->next, ds_.extend(from_node, c->S, pos+1), pos);
    } else if (nextTransition->type_ == Transition::Type::kEmpty) {
      from_node->dec_ref_count();
      ds_.try_mark_unused(from_node);
    } else if (nextTransition->type_ == Transition::Type::kSingleCapture) {
      visit(c->next, ds_.extend(from_node, c->S, pos+1), pos);
    } else if (nextTransition->type_ == Transition::Type::kDirectMultiCapture) {
      visit(d, from_node, pos, false);
      for (auto &capture : nextTransition->captures_) {
        from_node->inc_ref_count();
        visit(d, ds_.extend(from_node, capture->S, pos+1), pos);
      }
    } else {
      visit(d, ds_.extend(from_node, c->S, pos+1), pos);
      for (auto &capture : nextTransition->captures_) {
        from_node->inc_ref_count();
        visit(d, ds_.extend(from_node, capture->S, pos+1), pos);
      }
    }
  }

  current_states_.swap(new_states_);
}

inline void SegmentEvaluator::pass_outputs() {
  for (auto &state : reached_final_states_) {
    enumerator_.add_node(state->node);
    ds_.try_mark_unused(state->node);
    state->node = nullptr;
  }
  reached_final_states_.clear();
}

inline void SegmentEvaluator::visit(DState *ns, internal::ECS::Node* passed_node,
                                    int64_t pos, bool garbage_left) {
  if (ns->visited <= pos) {
    ns->node = passed_node;
    ns->visited = pos + 1;
    if (ns->accepting())
      reached_final_states_.push_back(ns);
    else
      new_states_.push_back(ns);
  } else {
    ns->node = ds_.unite(passed_node, ns->node, garbage_left);
  }
}

} // end namespace rematch