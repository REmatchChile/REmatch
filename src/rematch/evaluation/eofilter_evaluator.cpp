#include "eofilter_evaluator.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

EarlyOutputFilterEvaluator::EarlyOutputFilterEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d)
    : rgx_(rgx),
      enumerator_(rgx_),
      text_(d) {
  dfa().initState()->currentL->add(new internal::Node(internal::Node::Type::kBottom));
  macro_dfa_.set_as_init(macro_dfa_.add_state(dfa().initState()));
  current_state_ = &macro_dfa_.get_init_state();
  current_dstate_ = rawDFA().initState();
  reading(0, i_pos_);
}

Match_ptr EarlyOutputFilterEvaluator::next() {

  Enumerate:
  if(enumerator_.hasNext()) {
    return enumerator_.next();
  }

  char a;

  if(i_pos_ < text_->size()) {
    if(i_pos_ < i_max_)
      goto Evaluate;
    while(dfa_search()) {
      if(i_pos_ < i_min_) {
        pass_current_outputs();
        current_state_ = &macro_dfa_.get_init_state();
        reading(0, i_pos_);
      }
      for(i_pos_ = i_min_; i_pos_ < i_max_;) {
       Evaluate:
        a = (*text_)[i_pos_];
        a &= 0x7F;  // Only ASCII chars for now

        reading(a, i_pos_+1);

        ++i_pos_;

        if(current_state_->is_super_final()) {
          pass_current_outputs();
          if(i_pos_ == i_max_)
            i_min_ = i_pos_;
          goto Enumerate;
        }
      }
      i_min_ = i_pos_;
    }
    i_pos_ = i_min_;
    pass_outputs();
    goto Enumerate;
  }

  return nullptr;
}

FORCE_INLINE bool EarlyOutputFilterEvaluator::dfa_search() {
  char a;

  // TODO: Dejar un mínimo de procesamiento del documento (e.g 100 caracteres antes de salir)

  uint64_t it = i_min_;

  if(current_dstate_->isSuperFinal) {
    i_max_ = text_->size();
    return i_min_ < i_max_;
  }

  for(; it < text_->size(); ++it) {

    a = (char) (*text_)[it] & 0x7F;

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
      auto ns = current_dstate_->drop_super_finals();
      if(ns == nullptr)
        ns = rgx_.rawDetManager().compute_drop_super_finals(current_dstate_);
      current_dstate_ = ns;
      return true;
    }
  }

  i_min_ = text_->size();

  return false;
}


FORCE_INLINE void EarlyOutputFilterEvaluator::reading(char a, int64_t pos) {
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
    direct.from->currentL->reset_refs();
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


inline void EarlyOutputFilterEvaluator::pass_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isFinal) {
      enumerator_.addNodeList(state->currentL);
      state->currentL->reset_refs();
      memory_manager_.addPossibleGarbage(state->currentL->start());
      state->currentL->erase();
    }
  }
}

inline void EarlyOutputFilterEvaluator::pass_current_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isSuperFinal) {
      enumerator_.addNodeList(state->currentL);
      state->currentL->reset_refs();
      memory_manager_.addPossibleGarbage(state->currentL->start());
      state->currentL->erase();
    }
  }

  auto ns = current_state_->drop_super_finals();

  if(ns == nullptr)
    ns = rgx_.detManager().compute_drop_super_finals(current_state_);

  current_state_ = ns;
}

} // end namespace rematch