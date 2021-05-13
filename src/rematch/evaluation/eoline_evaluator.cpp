#include "eoline_evaluator.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

EarlyOutputLineEvaluator::EarlyOutputLineEvaluator(RegEx &rgx, std::shared_ptr<Document> d)
    : rgx_(rgx),
      enumerator_(rgx_),
      text_(d),
      i_pos_(0) {
  dfa().initState()->currentL->add(new internal::Node(internal::Node::Type::kBottom));
  macro_dfa_.set_as_init(macro_dfa_.add_state(dfa().initState()));
  current_line_ = text_->line_begin();
  current_char_ = (*current_line_).begin();
  current_state_ = &macro_dfa_.get_init_state();
  reading(0, i_pos_);
}

Match_ptr EarlyOutputLineEvaluator::next() {

 Enumerate:
  if(enumerator_.hasNext()) {
    return enumerator_.next();
  }

  char a;

  for( ;current_line_ != text_->line_end(); ) {
    if(current_char_ == (*current_line_).begin()) {
      if(!match(*current_line_)) {
        i_pos_ += (*current_line_).size();
        ++current_line_;
        current_char_ = (*current_line_).begin();
        continue;
      }
      current_char_ = (*current_line_).begin();
      current_state_ = &macro_dfa_.get_init_state();
      reading(0, i_pos_);
    }

    for(; current_char_ != (*current_line_).end();) {
      a = *current_char_;
      a &= 0x7F;  // Only ASCII chars for now

      reading(a, i_pos_+1);

      ++i_pos_;
      ++current_char_;

      if(current_state_->is_super_final()) {
        if(current_char_ != (*current_line_).end()) {
          pass_current_outputs();
          goto Enumerate;
        }
      }
    }
    ++current_line_;
    current_char_ = (*current_line_).begin();
    if(current_line_ != text_->line_end()) {
      pass_current_outputs();
      goto Enumerate;
    }
  }

  pass_outputs();

  if(enumerator_.hasNext()) {
    return enumerator_.next();
  }

  return nullptr;

}

bool EarlyOutputLineEvaluator::match(const std::string &s) {
  char a;
  DetState *nextState, *currentState;

  currentState = rawDFA().initState();

  int64_t it = 0;

  while( it < (int64_t)s.size() ) {

    a = (char) s[it] & 0x7F;

    // nextState is reached from currentState by reading the character
    auto nextTransition = currentState->next_transition(a);

    if(nextTransition == nullptr) // Then maybe a determinization is needed
      nextTransition = rgx_.rawDetManager().next_transition(currentState, a);

    if (nextTransition->type_ == 0)
      return false;

    nextState = nextTransition->direct_;

    if (nextState->isSuperFinal)
      return true;

    currentState = nextState;
    it++;
  }

  return currentState->isFinal;
}


void EarlyOutputLineEvaluator::reading(char a, int64_t pos) {
  auto nextTransition = current_state_->next_transition(a);

  if(nextTransition == nullptr) {
    nextTransition = rgx_.detManager().next_macro_transition(current_state_, a);
  }

  auto directs = nextTransition->directs();
  auto captures = nextTransition->captures();
  auto empties = nextTransition->empties();

  auto directs_sz = nextTransition->ndirects_;
  auto empties_sz = nextTransition->nempties_;
  auto captures_sz = nextTransition->ncaptures_;

  for(auto &dstate: nextTransition->next_state()->states())
    dstate->currentL->erase();

  for(size_t i=0; i < directs_sz; i++) {
    auto direct = directs[i];
    direct.to->currentL->append(direct.from->currentL);
    // direct.from->currentL->reset_refs();
  }

  for(size_t i=0; i < empties_sz; i++){
    auto empty = empties[i];
    empty->currentL->reset_refs();
    memory_manager_.addPossibleGarbage(empty->currentL->start());
    empty->currentL->erase();
  }

  for(size_t i=0; i < captures_sz; i++) {
    auto capture = captures[i];
    #ifndef NOPT_MEMORY_MANAGER
    internal::Node* new_node = memory_manager_.alloc(capture.S, pos,
                                                      capture.from->currentL->start(),
                                                      capture.from->currentL->end());
    #else
    internal::Node* new_node = new Node(capture.s, i_pos_+1, capture.from->currentL->head_,
                              capture.from->currentL->tail_)
    #endif

    capture.to->currentL->add(new_node);
  }

  current_state_ = nextTransition->next_state();
}


inline void EarlyOutputLineEvaluator::pass_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isFinal) {
      enumerator_.addNodeList(state->currentL);
      state->currentL->reset_refs();
      memory_manager_.addPossibleGarbage(state->currentL->start());
      state->currentL->erase();
    }
  }
}

inline void EarlyOutputLineEvaluator::pass_current_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isSuperFinal) {
      enumerator_.addNodeList(state->currentL);
      state->currentL->reset_refs();
      memory_manager_.addPossibleGarbage(state->currentL->start());
      state->currentL->erase();
    }
  }
}

} // end namespace rematch