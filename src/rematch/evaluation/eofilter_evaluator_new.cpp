#include "eofilter_evaluator_new.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

EarlyOutputFilterEvaluatorNew::EarlyOutputFilterEvaluatorNew(RegEx &rgx, std::shared_ptr<StrDocument> d)
    : rgx_(rgx),
      enumerator_(rgx_),
      text_(d) {
  dfa().initState()->currentNode = ds_.bottom_node();
  macro_dfa_.set_as_init(macro_dfa_.add_state(dfa().initState()));
  current_state_ = &macro_dfa_.get_init_state();
  current_dstate_ = rawDFA().initState();
  reading(0, -1);
}

Match_ptr EarlyOutputFilterEvaluatorNew::next() {

  Enumerate:
  if(enumerator_.has_next()) {
    return enumerator_.next();
  }

  char a;

  if(i_pos_ < text_->size()) {
    if(i_pos_ < i_max_) // If pos not at max then we have to keep evaluating
      goto Evaluate;
    while(dfa_search()) {
      if(i_pos_ < i_min_) { // If pos not at min after dfa_search then reset runs
        pass_current_outputs();
        current_state_ = &macro_dfa_.get_init_state();
        reading(0, i_min_-1);
      }
      for(i_pos_ = i_min_; i_pos_ < i_max_;) {
       Evaluate:
        a = (*text_)[i_pos_];
        a &= 0x7F;  // Only ASCII chars for now

        reading(a, i_pos_);

        ++i_pos_;

        if(current_state_->is_super_final()) {
          pass_current_outputs();
          if(i_pos_ == i_max_) // If pos at max then set min to pos
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

FORCE_INLINE bool EarlyOutputFilterEvaluatorNew::dfa_search() {
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


FORCE_INLINE void EarlyOutputFilterEvaluatorNew::reading(char a, int64_t pos) {
  auto nextTransition = current_state_->next_transition(a);

  if(nextTransition == nullptr) {
    nextTransition = rgx_.detManager().next_macro_transition(current_state_, a);
  }

  auto first_directs = nextTransition->first_directs();
  auto repeat_directs = nextTransition->repeat_directs();
  auto first_captures = nextTransition->first_captures();
  auto repeat_captures = nextTransition->repeat_captures();
  auto empties = nextTransition->empties();

  auto first_directs_sz = nextTransition->nfirstdirects_;
  auto repeat_directs_sz = nextTransition->nrepeatdirects_;
  auto first_captures_sz = nextTransition->nfirstcaptures_;
  auto repeat_captures_sz = nextTransition->nrepeatcaptures_;
  auto empties_sz = nextTransition->nempties_;

  for(size_t i=0; i < first_directs_sz; i++) {
    auto direct = first_directs[i];
    direct.to->currentNode = direct.from->currentNode;
  }

  for(size_t i=0; i < first_captures_sz; i++) {
    auto capture = first_captures[i];
    capture.to->currentNode = ds_.extend(capture.from->currentNode, capture.S, pos+1);
  }

  for(size_t i=0; i < repeat_directs_sz; i++) {
    auto direct = repeat_directs[i];
    direct.to->currentNode = ds_.unite(direct.from->currentNode, direct.to->currentNode);
  }

  for(size_t i=0; i < repeat_captures_sz; i++) {
    auto capture = repeat_captures[i];
    capture.to->currentNode = ds_.unite(ds_.extend(capture.from->currentNode, capture.S, pos+1), capture.to->currentNode);
  }

  for(size_t i=0; i < empties_sz; i++) {
    auto empty = empties[i];
    ds_.try_mark_unused(empty->currentNode);
    empty->currentNode = nullptr;
  }

  current_state_ = nextTransition->next_state();
}


inline void EarlyOutputFilterEvaluatorNew::pass_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isFinal) {
      enumerator_.add_node(state->currentNode);
      ds_.try_mark_unused(state->currentNode);
      state->currentNode = nullptr;
    }
  }
}

inline void EarlyOutputFilterEvaluatorNew::pass_current_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isSuperFinal) {
      enumerator_.add_node(state->currentNode);
      ds_.try_mark_unused(state->currentNode);
      state->currentNode = nullptr;
    }
  }

  auto ns = current_state_->drop_super_finals();

  if(ns == nullptr)
    ns = rgx_.detManager().compute_drop_super_finals(current_state_);

  current_state_ = ns;
}

} // end namespace rematch