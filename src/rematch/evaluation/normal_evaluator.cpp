#include "normal_evaluator.hpp"

#include "automata/dfa/dstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

NormalEvaluator::NormalEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d,
                                 Anchor a, EvalStats &e)
    : rgx_(rgx), enumerator_(rgx_), text_(d), anchor_(a), stats_(e) {

  eva_ = std::make_unique<ExtendedVA>(rgx_.logicalVA(), anchor_);
  sva_ = std::make_unique<SearchVA>(rgx_.logicalVA(), anchor_);

  dfa_ = std::make_unique<DFA>(*eva_);
  sdfa_ = std::make_unique<SearchDFA>(*sva_);

  stats_.eva_size = eva_->size();
  stats_.sva_size = sva_->size();

  current_dstate_ = sdfa_->initial_state();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

Match_ptr NormalEvaluator::next() {

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

void NormalEvaluator::init_evaluation_phase(int64_t pos) {
  dfa_->init_state()->pass_node(ds_.bottom_node());

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

bool NormalEvaluator::evaluation_phase() {
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

FORCE_INLINE void NormalEvaluator::reading(char a, int64_t pos) {

  new_states_.clear();

  for (auto &p : current_states_) {
    auto nextTransition = p->next_transition(a);

    if (nextTransition == nullptr) {
      nextTransition = dfa_->next_transition(p, a);
    }

    // Decrease the refcount before checking the transition. This is for
    // correct identification of the case of an empty transition, to mark the
    // node as unused.
    p->node->dec_ref_count();

    auto *c = nextTransition->capture_;
    auto *d = nextTransition->direct_;

    internal::ECS::Node* from_node;
    #ifdef NOPT_CROSSPROD
      if(p->visited <= pos)
        from_node = p->node;
      else
        from_node = p->old_node;
    #else
      from_node = p->node;
    #endif


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

inline void NormalEvaluator::pass_outputs() {
  for (auto &state : reached_final_states_) {
    enumerator_.add_node(state->node);
    #ifndef NOPT_EARLYOUTPUT
    ds_.try_mark_unused(state->node);
    #endif
    state->node = nullptr;
  }
  reached_final_states_.clear();
}

inline void NormalEvaluator::visit(DState *ns, internal::ECS::Node* passed_node,
                                    int64_t pos, bool garbage_left) {
  if (ns->visited <= pos) {
    #ifdef NOPT_CROSSPROD
    ns->old_node = ns->node;
    #endif
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