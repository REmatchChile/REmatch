#include "normal_evaluator.hpp"

#include "automata/dfa/dstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

NormalEvaluator::NormalEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d,
                                 Anchor a)
    : rgx_(rgx), enumerator_(rgx_), text_(d), anchor_(a) {

  eva_ = std::make_unique<ExtendedVA>(rgx_.logicalVA(), anchor_);
  sva_ = std::make_unique<SearchVA>(rgx_.logicalVA(), anchor_);

  dfa_ = std::make_unique<DFA>(*eva_);
  sdfa_ = std::make_unique<SearchDFA>(*sva_);

  current_dstate_ = sdfa_->initial_state();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

Match_ptr NormalEvaluator::next() {

Enumeration:
  if (enumerator_.has_next())
    return enumerator_.next();

  if (evaluation_phase()) { // Then there's output to enumerate
    pass_outputs();
    goto Enumeration;
  }

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

    if (!reached_final_states_.empty())
      return true; // On-line output
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

    if (nextTransition->type_ == Transition::Type::kDirect) {
      visit(d, p->node, pos);
    } else if (nextTransition->type_ == Transition::Type::kDirectSingleCapture) {
      visit(d, p->node, pos, false);
      p->node->inc_ref_count();
      visit(c->next, ds_.extend(p->node, c->S, pos+1), pos);
    } else if (nextTransition->type_ == Transition::Type::kEmpty) {
      p->node->dec_ref_count();
      ds_.try_mark_unused(p->node);
    } else if (nextTransition->type_ == Transition::Type::kSingleCapture) {
      visit(c->next, ds_.extend(p->node, c->S, pos+1), pos);
    } else if (nextTransition->type_ == Transition::Type::kDirectMultiCapture) {
      visit(d, p->node, pos, false);
      for (auto &capture : nextTransition->captures_) {
        p->node->inc_ref_count();
        visit(d, ds_.extend(p->node, capture->S, pos+1), pos);
      }
    } else {
      visit(d, ds_.extend(p->node, c->S, pos+1), pos);
      for (auto &capture : nextTransition->captures_) {
        p->node->inc_ref_count();
        visit(d, ds_.extend(p->node, capture->S, pos+1), pos);
      }
    }
  }

  current_states_.swap(new_states_);
}

inline void NormalEvaluator::pass_outputs() {
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