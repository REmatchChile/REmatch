#include "normal_evaluator.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "util/timer.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

NormalEvaluator::NormalEvaluator(RegEx &rgx, std::shared_ptr<Document> d)
    : rgx_(rgx),
      enumerator_(rgx_),
      text_(d),
      i_pos_(0) {
  dfa().initState()->currentL->add(new internal::Node(internal::Node::Type::kBottom));
  macro_dfa_.set_as_init(macro_dfa_.add_state(dfa().initState()));
  current_state_ = &macro_dfa_.get_init_state();
  current_char_ = text_->begin();
  reading(0, -1);
}

Match_ptr NormalEvaluator::next() {

  if(enumerator_.hasNext()) {
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


  if(enumerator_.hasNext()) {
    return enumerator_.next();
  }

  return nullptr;

}


void NormalEvaluator::reading(char a, int64_t pos) {
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
    NormalEvaluator::memory_manager_.addPossibleGarbage(empty->currentL->start());
    empty->currentL->erase();
  }

  for(size_t i=0; i < captures_sz; i++) {
    auto capture = captures[i];
    #ifndef NOPT_MEMORY_MANAGER
    internal::Node* new_node = NormalEvaluator::memory_manager_.alloc(capture.S,
                                                      pos+1,
                                                      capture.from->currentL->start(),
                                                      capture.from->currentL->end());
    #else
    internal::Node* new_node = new Node(capture.s, pos+1, capture.from->currentL->start(),
                              capture.from->currentL->end())
    #endif

    capture.to->currentL->add(new_node);
  }

  current_state_ = nextTransition->next_state();
}



inline void NormalEvaluator::pass_current_outputs() {
  for(auto &state: current_state_->states()) {
    if(state->isFinal) {
      enumerator_.addNodeList(state->currentL);
      state->currentL->reset_refs();
      memory_manager_.addPossibleGarbage(state->currentL->start());
      state->currentL->erase();
    }
  }
}

} // end namespace rematch