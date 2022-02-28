#include "segment_evaluator.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"


#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

SegmentEvaluator::SegmentEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d, Anchor a)
    : rgx_(rgx),
      enumerator_(rgx_),
      text_(d),
      anchor_(a) {

  dfa_ = std::make_unique<DFA>(rgx_.evalVA(), anchor_);
  sdfa_ = std::make_unique<SearchDFA>(rgx_.searchVA(), anchor_);

  current_dstate_ = sdfa_->initial_state();
  // Initialize evaluation for position 0
  init_evaluation_phase(0);
}

Match_ptr SegmentEvaluator::next() {

 Enumeration:
  if(enumerator_.has_next())
    return enumerator_.next();

 Evaluation:
  if( i_pos_ < i_max_ ) {
    if( i_pos_ < i_min_ ) // Then we know that the current runs are useless
      init_evaluation_phase(i_min_);
    if(evaluation_phase()) { // Then there's output to enumerate
      pass_outputs();
      goto Enumeration;
    }
  }

 // Searching
  init_searching_phase();
  if(searching_phase())
    goto Evaluation;

  return nullptr;
}

void SegmentEvaluator::init_evaluation_phase(int64_t pos) {
  dfa_->initState()->pass_node(ds_.bottom_node());
  current_states_.clear();
  current_states_.push_back(dfa_->initState());
  reading(0, pos-1); // Positions are offseted by -1 because captures come before filters
  i_pos_ = pos;
}


bool SegmentEvaluator::evaluation_phase() {
  while(i_pos_ < i_max_) {
    char a = (*text_)[i_pos_];
    a &= 0x7F;  // Only ASCII chars for now

    reading(a, i_pos_);

    ++i_pos_;

    if(!reached_final_states_.empty())
      return true; // On-line output
  }

  return false;
}


void SegmentEvaluator::init_searching_phase() { }


bool SegmentEvaluator::searching_phase() {

  i_min_ = i_src_;
  i_max_ = i_src_;

  for(; i_src_ < text_->size(); ++i_src_) {

    char a = (char) (*text_)[i_src_] & 0x7F;

    // nextState is reached from currentState by reading the character
    DState* next_state = current_dstate_->next_state(a);

    if(next_state == nullptr) // Then maybe a determinization is needed
      next_state = sdfa_->next_state(current_dstate_, a);

    current_dstate_ = next_state;

    if(current_dstate_->accepting())
      i_max_ = i_src_;

    if((current_dstate_->initial() || i_src_ == text_->size()) && i_min_ < i_max_)
      return true;

    if(current_dstate_->initial())
      i_min_ = i_src_;
  }

  i_min_ = text_->size();

  return false;
}


FORCE_INLINE void SegmentEvaluator::reading(char a, int64_t pos) {

  new_states_.clear();

  for(auto &curr_state: current_states_) {
    auto nextTransition = curr_state->next_transition(a);

    if(nextTransition == nullptr) {
      nextTransition = dfa_->next_transition(curr_state, a);
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


inline void SegmentEvaluator::pass_outputs() {
  for(auto &state: reached_final_states_) {
    enumerator_.add_node(state->currentNode);
    ds_.try_mark_unused(state->currentNode);
    state->currentNode = nullptr;
  }
  reached_final_states_.clear();
}

inline void SegmentEvaluator::visit_direct(DetState* from, DetState* to, int64_t pos) {
  if(to->visited <= pos) {
    to->pass_node(from->currentNode);
    to->visited = pos+1;
    if(to->isFinal) reached_final_states_.push_back(to);
    else new_states_.push_back(to);
  } else {
    // Decrease the refcount, as the node at reached state won't be pointed by that
    // state anymore, only by the structure internally.
    to->currentNode->dec_ref_count();
    to->pass_node(ds_.unite(from->currentNode, to->currentNode));
  }
}

inline void SegmentEvaluator::visit_capture(DetState* cs, Capture* c, int64_t pos) {
  DetState* to = c->next;
  if(to->visited <= pos) {
    to->pass_node(ds_.extend(cs->currentNode, c->S, pos+1));
    to->visited = pos+1;
    if(to->isFinal) reached_final_states_.push_back(to);
    else new_states_.push_back(to);
  } else {
    // Decrease the refcount, as the node at reached state won't be pointed by that
    // state anymore, only by the structure internally.
    to->currentNode->dec_ref_count();
    to->pass_node(ds_.unite(ds_.extend(cs->currentNode, c->S, pos+1), c->next->currentNode));
  }
}

} // end namespace rematch