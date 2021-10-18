#include "eofilter_evaluator_new_v2.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"



#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

EarlyOutputFilterEvaluatorNewV2::EarlyOutputFilterEvaluatorNewV2(RegEx &rgx, std::shared_ptr<StrDocument> d)
    : rgx_(rgx),
      enumerator_(rgx_),
      text_(d) {
  current_dstate_ = rawDFA().initState();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

Match_ptr EarlyOutputFilterEvaluatorNewV2::next() {

 Enumeration:
  if(enumerator_.has_next())
    return enumerator_.next();

 Evaluation:
  if( i_pos_ < i_max_ ) {
    if( i_pos_ < i_min_ ) // Then we know that the current runs are useless
      init_evaluation_phase(i_min_);
    if(evaluation_phase()) { // Then there's output to enumerate
      pass_current_outputs();
      goto Enumeration;
    }
  }

 // Searching
  init_searching_phase();
  if(searching_phase())
    goto Evaluation;

  return nullptr;
}

void EarlyOutputFilterEvaluatorNewV2::init_evaluation_phase(int64_t pos) {
  dfa().initState()->pass_node(ds_.bottom_node());
  current_states_.clear();
  current_states_.push_back(dfa().initState());
  reading(0, pos-1); // Positions are offseted by -1 because captures come before filters
  i_pos_ = pos;
}


bool EarlyOutputFilterEvaluatorNewV2::evaluation_phase() {
  while(i_pos_ < i_max_) {
    char a = (*text_)[i_pos_];
    a &= 0x7F;  // Only ASCII chars for now

    reading(a, i_pos_);

    ++i_pos_;

    if(!super_finals_.empty())
      return true; // On-line output
  }

  return false;
}


void EarlyOutputFilterEvaluatorNewV2::init_searching_phase() { i_min_ = i_max_; }


bool EarlyOutputFilterEvaluatorNewV2::searching_phase() {
  uint64_t it = i_max_;

  // .*a+.*

  // If init_state_->isSuperFinal
  if(current_dstate_->isSuperFinal) {
    i_max_ = text_->size();
    return i_min_ < i_max_;
  }

  for(; it < text_->size(); ++it) {

    char a = (char) (*text_)[it] & 0x7F;

    // nextState is reached from currentState by reading the character
    auto nextTransition = current_dstate_->next_transition(a);

    if(nextTransition == nullptr) // Then maybe a determinization is needed
      nextTransition = rgx_.rawDetManager().next_transition(current_dstate_, a);

    current_dstate_ = nextTransition->direct_;

    if(current_dstate_->isOnlyInit) {
      i_min_ = it + 1;
    }

    if(current_dstate_->isSuperFinal) {
      i_max_ = it + 1;
      DetState* ns = current_dstate_->drop_super_finals();
      if(ns == nullptr)
        ns = rgx_.rawDetManager().compute_drop_super_finals(current_dstate_);
      current_dstate_ = ns;
      return true;
    }
  }

  i_min_ = text_->size();

  return false;
}


FORCE_INLINE void EarlyOutputFilterEvaluatorNewV2::reading(char a, int64_t pos) {

  new_states_.clear();

  for(auto &curr_state: current_states_) {
    auto nextTransition = curr_state->next_transition(a);

    if(nextTransition == nullptr) {
      nextTransition = rgx_.detManager().next_transition(curr_state, a);
    }

    // Decrease the refcount before checking the transition. This is for
    // correct identification of the case of an empty transition, to mark the
    // node as unused.
    curr_state->currentNode->dec_ref_count();

    if(nextTransition->type_ == Transition::Type::kDirect) {
      visit_direct(curr_state, nextTransition->direct_, pos);
    } else if(nextTransition->type_ == Transition::Type::kDirectSingleCapture) {
      visit_direct(curr_state, nextTransition->direct_, pos);
      visit_capture(curr_state, nextTransition->capture_, pos);
    } else if(nextTransition->type_ == Transition::Type::kEmpty) {
      ds_.try_mark_unused(curr_state->currentNode);
    } else if(nextTransition->type_ == Transition::Type::kSingleCapture) {
      visit_capture(curr_state, nextTransition->capture_, pos);
    } else if(nextTransition->type_ == Transition::Type::kDirectMultiCapture) {
      visit_direct(curr_state, nextTransition->direct_, pos);
      for(auto &capture: nextTransition->captures_) {
        visit_capture(curr_state, capture, pos);
      }
    } else {
      for(auto &capture: nextTransition->captures_) {
        visit_capture(curr_state, capture, pos);
      }
    }
  }

  current_states_.swap(new_states_);
}


inline void EarlyOutputFilterEvaluatorNewV2::pass_outputs() {
  for(auto &state: current_states_) {
    if(state->isFinal) {
      enumerator_.add_node(state->currentNode);
      ds_.try_mark_unused(state->currentNode);
      state->currentNode = nullptr;
    }
  }
}

inline void EarlyOutputFilterEvaluatorNewV2::pass_current_outputs() {
  for(auto &state: super_finals_) {
    enumerator_.add_node(state->currentNode);
    ds_.try_mark_unused(state->currentNode);
    state->currentNode = nullptr;
  }
  super_finals_.clear();
}

inline void EarlyOutputFilterEvaluatorNewV2::visit_direct(DetState* from, DetState* to, int64_t pos) {
  if(to->visited <= pos) {
    to->pass_node(from->currentNode);
    to->visited = pos+1;
    if(to->isSuperFinal) super_finals_.push_back(to);
    else new_states_.push_back(to);
  } else {
    // Decrease the refcount, as the node at reached state won't be pointed by that
    // state anymore, only by the structure internally.
    to->currentNode->dec_ref_count();
    to->pass_node(ds_.unite(from->currentNode, to->currentNode));
  }
}

inline void EarlyOutputFilterEvaluatorNewV2::visit_capture(DetState* cs, Capture* c, int64_t pos) {
  DetState* to = c->next;
  if(c->next->visited <= pos) {
    to->pass_node(ds_.extend(cs->currentNode, c->S, pos+1));
    c->next->visited = pos+1;
    if(c->next->isSuperFinal) super_finals_.push_back(c->next);
    else new_states_.push_back(c->next);
  } else {
    // Decrease the refcount, as the node at reached state won't be pointed by that
    // state anymore, only by the structure internally.
    to->currentNode->dec_ref_count();
    to->pass_node(ds_.unite(ds_.extend(cs->currentNode, c->S, pos+1), c->next->currentNode));
  }
}

} // end namespace rematch