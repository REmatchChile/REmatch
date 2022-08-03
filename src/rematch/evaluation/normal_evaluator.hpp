#ifndef EVALUATION__NORMAL_EVALUATOR_HPP
#define EVALUATION__NORMAL_EVALUATOR_HPP

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "evaluation/evaluator.hpp"
#include "match.hpp"
#include "regex/regex.hpp"
#include "structs/ecs/enumerator.hpp"

#include "automata/dfa/dfa.hpp"
#include "automata/nfa/uva.hpp"
#include "automata/dfa/sdfa.hpp"

#include "automata/dfa/transition.hpp"
#include "evaluation/document/document.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

#include "evaluation/stats.hpp"

namespace rematch {

template <typename A>
class NormalEvaluator : public Evaluator {

public:
  NormalEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d,
                  Anchor a, EvalStats &e);

  Match_ptr next() override;

  bool is_ambiguous() const override { return eva_->is_ambiguous(); }

private:
  inline void reading(char a, int64_t i);

  inline void visit(abstract::DState *direct, ECS::Node* n, int64_t pos, bool gleft=true);

  // Executes the evaluation phase. Returns true if there is an output to
  // enumerate but it didn't reach the end of the search interval. Returns
  // false otherwise.
  bool evaluation_phase();
  void init_evaluation_phase(int64_t init_from);

  inline void pass_current_outputs();
  inline void pass_outputs();

  std::shared_ptr<ExtendedVA> eva_;
  std::unique_ptr<SearchVA> sva_;

  std::unique_ptr<A> dfa_;        // Normal DFA
  std::unique_ptr<SearchDFA> sdfa_; // Search DFA

  RegEx &rgx_;

  Enumerator enumerator_;

  ECS ds_; // DAG structure

  std::shared_ptr<StrDocument> text_;

  Anchor anchor_;

  std::vector<abstract::DState*> current_states_;
  std::vector<abstract::DState*> new_states_;

  std::vector<abstract::DState*> reached_final_states_;

  SDState *current_dstate_;

  static const size_t kSizeMaxOutputBuffer = 100;

  uint64_t i_pos_ = 0;
  uint64_t i_src_ = 0;
  uint64_t i_min_ = 0;
  uint64_t i_max_ = 0;

  size_t out_buf_counter = 0;

  EvalStats &stats_;
}; // end class Evaluator

///////////////////////////////
//        DEFINITIONS        //
///////////////////////////////

template <typename A>
NormalEvaluator<A>::NormalEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d,
                                    Anchor a, EvalStats &e)
    : rgx_(rgx), enumerator_(rgx_), text_(d), anchor_(a), stats_(e) {

  eva_ = rgx_.extendedVA();
  sva_ = std::make_unique<SearchVA>(rgx_.logicalVA(), anchor_);

  dfa_ = std::make_unique<A>(*eva_);
  sdfa_ = std::make_unique<SearchDFA>(*sva_);

  stats_.eva_size = eva_->size();
  stats_.sva_size = sva_->size();

  current_dstate_ = sdfa_->initial_state();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

template <typename A>
Match_ptr NormalEvaluator<A>::next() {

 Enumeration:
  if (enumerator_.has_next())
    return enumerator_.next();

  if (evaluation_phase()) { // Then there's output to enumerate
    #ifndef NOPT_EARLYOUTPUT
    pass_outputs();
    #endif
    goto Enumeration;
  }

  stats_.dfa_size = dfa_->size();
  stats_.sdfa_size = sdfa_->size();

  return nullptr;
}

template <typename A>
void NormalEvaluator<A>::init_evaluation_phase(int64_t pos) {
  dfa_->init_state()->set_node(ds_.bottom_node());

  current_states_.clear();

  for (auto &elem : dfa_->init_eval_states()) {
    typename A::State *q0 = elem.first;
    std::bitset<32> S = elem.second;
    if (S != 0)
      visit(q0, ds_.extend(dfa_->init_state()->node(), S, pos), pos-1);
    current_states_.push_back(elem.first);
  }

  i_pos_ = pos;
}

template <typename A>
bool NormalEvaluator<A>::evaluation_phase() {
  while (i_pos_ < text_->size()) {
    char a = (*text_)[i_pos_];
    a &= 0x7F; // Only ASCII chars for now

    reading(a, i_pos_);

    ++i_pos_;

    if (!reached_final_states_.empty()) {
      #ifdef NOPT_EARLYOUTPUT
      pass_outputs();
      #else
      return true; // On-line output
      #endif
    }
    #ifdef NOPT_EARLYOUTPUT
    if(i_pos_ >= text_->size())
      return true;
    #endif
  }

  return false;
}

template <typename A>
inline void NormalEvaluator<A>::reading(char a, int64_t pos) {
  new_states_.clear();

  for (auto &p : current_states_) {
    auto nextTransition = p->next_transition(a);

    if (!nextTransition) {
      nextTransition = dfa_->next_transition(p, a);
    }

    auto c = nextTransition->capture_;
    auto *d = nextTransition->direct_;

    ECS::Node* from_node;
    #ifdef NOPT_CROSSPROD
      if(p->visited <= pos)
        from_node = p->node;
      else
        from_node = p->old_node;
    #else
      from_node = p->node();
    #endif

    switch(nextTransition->type_) {
      case Transition::Type::kDirect:
        visit(d, from_node, pos);
        break;
      case Transition::Type::kDirectSingleCapture:
        visit(d, from_node, pos, false);
        from_node->inc_ref_count();
        visit(c.next, ds_.extend(from_node, c.S, pos+1), pos);
        break;
      case Transition::Type::kEmpty:
        from_node->dec_ref_count();
        ds_.try_mark_unused(from_node);
        break;
      case Transition::Type::kSingleCapture:
        visit(c.next, ds_.extend(from_node, c.S, pos+1), pos);
        break;
      case Transition::Type::kMultiDirect:
        visit(d, from_node, pos, false);
        for(auto &q: nextTransition->directs_) {
          from_node->inc_ref_count();
          visit(q, from_node, pos, false);
        }
        break;
      case Transition::Type::kMultiDirectSingleCapture:
        visit(d, from_node, pos, false);
        for(auto &q: nextTransition->directs_) {
          from_node->inc_ref_count();
          visit(q, from_node, pos, false);
        }
        from_node->inc_ref_count();
        visit(c.next, ds_.extend(from_node, c.S, pos+1), pos);
        break;
      case Transition::Type::kDirectMultiCapture:
        visit(d, from_node, pos, false);
        for (auto &capture : nextTransition->captures_) {
          from_node->inc_ref_count();
          visit(d, ds_.extend(from_node, capture.S, pos+1), pos);
        }
        break;
      case Transition::Type::kMultiDirectMultiCapture:
        visit(d, from_node, pos, false);
        for(auto &q: nextTransition->directs_) {
          from_node->inc_ref_count();
          visit(q, from_node, pos, false);
        }
        for (auto &capture : nextTransition->captures_) {
          from_node->inc_ref_count();
          visit(capture.next, ds_.extend(from_node, capture.S, pos+1), pos);
        }
        break;
      default:
        visit(d, ds_.extend(from_node, c.S, pos+1), pos);
        for (auto &capture : nextTransition->captures_) {
          from_node->inc_ref_count();
          visit(capture.next, ds_.extend(from_node, capture.S, pos+1), pos);
        }
        break;
      }
  }

  current_states_.swap(new_states_);
}

template <typename A>
inline void NormalEvaluator<A>::pass_outputs() {
  for (auto &state : reached_final_states_) {
    enumerator_.add_node(state->node());
    #ifndef NOPT_EARLYOUTPUT
    ds_.try_mark_unused(state->node());
    #endif
    state->set_node(nullptr);
  }
  reached_final_states_.clear();
}

template <typename A>
inline void NormalEvaluator<A>::visit(abstract::DState *ns,
                                      ECS::Node* passed_node,
                                      int64_t pos, bool garbage_left) {
  if (ns->visited() <= pos) {
    #ifdef NOPT_CROSSPROD
    ns->old_node = ns->node;
    #endif
    ns->set_node(passed_node);
    ns->set_visited(pos + 1);
    if (ns->accepting())
      reached_final_states_.push_back(ns);
    else
      new_states_.push_back(ns);
  } else {
    ns->set_node(ds_.unite(passed_node, ns->node(), garbage_left));
  }
}

} // namespace rematch

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP