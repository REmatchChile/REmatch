#include "normal_evaluator_new.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

NormalEvaluatorNew::NormalEvaluatorNew(RegEx &rgx, std::shared_ptr<Document> d)
    : rgx_(rgx),
      enumerator_(rgx),
      text_(d),
      i_pos_(0) {
  dfa().initState()->currentNode = ds_.bottom_node();
  macro_dfa_.set_as_init(macro_dfa_.add_state(dfa().initState()));
  current_state_ = &macro_dfa_.get_init_state();
  current_char_ = text_->begin();
  reading(0, -1);
}

Match_ptr NormalEvaluatorNew::next() {

  if(enumerator_.has_next()) {
    return enumerator_.next();
  }

  char a;

  if(current_char_ != text_->end()) {
    for(; current_char_ != text_->end(); ++current_char_) {
      a = *current_char_;
      a &= 0x7F;  // Only ASCII chars for now

      reading(a, i_pos_);

      i_pos_++;
    }
    pass_current_outputs();
  }

  if(enumerator_.has_next()) {
    return enumerator_.next();
  }

  return nullptr;

}


void NormalEvaluatorNew::reading(char a, int64_t pos) {
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

inline void NormalEvaluatorNew::pass_current_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isFinal) {
      enumerator_.add_node(state->currentNode);
      ds_.try_mark_unused(state->currentNode);
      state->currentNode = nullptr;
    }
  }
}

} // end namespace rematch