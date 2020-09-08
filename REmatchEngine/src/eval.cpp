#include "eval.hpp"

#include "automata/detstate.hpp"
#include "memmanager.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

MemManager Evaluator::memory_manager_{};

Evaluator::Evaluator(RegEx &rgx, std::istream& input,
                     uint8_t flags)
    : rgx_(rgx),
      text_(std::make_unique<FileDocument>(input)),
      early_output_(flags & kEarlyOutput),
      line_by_line_(flags & kLineByLine),
      document_ended_(false),
      direct_text_(false),
      i_pos_(0),
      i_start_(0),
      nlines_(0),
      capture_counter_(0),
      reading_counter_(0),
      direct_c_(0),
      single_c_(0),
      direct_single_c_(0),
      direct_multi_c_(0),
      multi_c_(0),
      empty_c_(0),
      det_c_(0) {
  init();
}

Evaluator::Evaluator(RegEx &rgx, const std::string &text,
                     uint8_t flags)
    : rgx_(rgx),
      text_(std::make_unique<StrDocument>(text)),
      early_output_(flags & kEarlyOutput),
      line_by_line_(flags & kLineByLine),
      document_ended_(false),
      direct_text_(true),
      i_pos_(0),
      i_start_(0),
      nlines_(0),
      capture_counter_(0),
      reading_counter_(0),
      direct_c_(0),
      single_c_(0),
      direct_single_c_(0),
      direct_multi_c_(0),
      multi_c_(0),
      empty_c_(0),
      det_c_(0) {
  init();
}

void Evaluator::init() {
  std::string str_ = "";
  enumerator_ = std::make_unique<Enumerator>(rgx_, str_);
  if(line_by_line_) {
    text_->getline(line_);
    line_ += '\n';
    nlines_++;
  }
  initAutomaton(i_pos_);
}

void Evaluator::initAutomaton(int64_t i) {
  int64_t pos;
  if(i == 0) {
    DFA().initState()->currentL->add(Evaluator::memory_manager_.alloc());
    pos = i-1;
  } else {
    pos = i;
  }

  current_states_.clear();
  current_states_.push_back(DFA().initState());

  new_states_.clear();
  NodeList* prevList;

  static void (Evaluator::*visits[])(int64_t, Transition*, NodeList *) = {
    &Evaluator::visitEmpty,
    &Evaluator::visitDirect,
    &Evaluator::visitSingleCapture,
    &Evaluator::visitDirectSingleCapture,
    &Evaluator::visitMultiCapture,
    &Evaluator::visitDirectMultiCapture,
  };

  for(auto &currentState: current_states_) {
    prevList = currentState->currentL;
    // nextCaptures are reached from currentState by reading the character
    auto nextTransition = currentState->next_transition(0);

    if(nextTransition == nullptr) { // Then maybe a determinization is needed
      nextTransition = rgx_.detManager().next_transition(currentState, 0);
      det_c_++;
    }

    (this->*visits[nextTransition->type_])(pos, nextTransition, prevList);
  }

  current_states_.swap(new_states_);
}

Match_ptr Evaluator::next() {
  if(enumerator_->hasNext())
      return enumerator_->next();

  while (!document_ended_) {
    char a;
    output_nodelist_.reset();

    while((i_pos_-i_start_) < (int64_t)line_.size()) { // Main search loop

      a = line_[i_pos_-i_start_];

      a &= 0x7F;  // Only ASCII chars for now

      new_states_.clear();
      NodeList* prevList;

      static void (Evaluator::*visits[])(int64_t, Transition*, NodeList *) = {
        &Evaluator::visitEmpty,
        &Evaluator::visitDirect,
        &Evaluator::visitSingleCapture,
        &Evaluator::visitDirectSingleCapture,
        &Evaluator::visitMultiCapture,
        &Evaluator::visitDirectMultiCapture,
      };

      for(auto &currentState: current_states_) {
        prevList = currentState->currentL;
        // nextCaptures are reached from currentState by reading the character
        auto nextTransition = currentState->next_transition(a);

        if(nextTransition == nullptr) { // Then maybe a determinization is needed
          nextTransition = rgx_.detManager().next_transition(currentState, a);
          det_c_++;
        }

        (this->*visits[nextTransition->type_])(i_pos_, nextTransition, prevList);
      }

      current_states_.swap(new_states_);

      i_pos_++;
    }

    for(auto &state: current_states_) {
      if(state->isFinal)
        output_nodelist_.append(state->currentL);
    }
    if(!output_nodelist_.empty()) {
      enumerator_->addNodeList(output_nodelist_);
      Evaluator::memory_manager_.addPossibleGarbage(output_nodelist_.head);
    }

    if((i_pos_-i_start_) == (int64_t)line_.size()) {
      while(!(document_ended_ = !((bool) text_->getline(line_)))) {
        line_ += '\n';
        i_start_ = i_pos_;
        nlines_++;

        if(!match()) {
          i_pos_ += line_.size() - 1;
          continue;
        }
        else {
          initAutomaton(i_start_);
          break;
        }
      }
    }
  }

    if(enumerator_->hasNext())
      return enumerator_->next();

    return nullptr;

}

bool Evaluator::match() {
  char a;
  DetState *nextState, *currentState;

  currentState = rawDFA().initState();

  int64_t it = 0;

  while( it < (int64_t)line_.size() ) {

    a = (char) line_[it] & 0x7F;

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


FORCE_INLINE void Evaluator::visitEmpty(int64_t i, Transition *t, NodeList *prev_list) {
  empty_c_++;
  prev_list->resetRefs();
  Evaluator::memory_manager_.addPossibleGarbage(prev_list->head);
}

FORCE_INLINE void Evaluator::visitDirect(int64_t i, Transition *t, NodeList *prev_list) {

  reading_counter_++;

  direct_c_++;

  if(t->direct_->visited <= i+1) {
    t->direct_->visited = i+2;

    // Lazy copy
    t->direct_->currentL->head = prev_list->head;
    t->direct_->currentL->tail = prev_list->tail;

    new_states_.push_back(t->direct_);
  } else {
    t->direct_->currentL->append(prev_list);
  }
}

FORCE_INLINE void Evaluator::visitSingleCapture(int64_t i, Transition *t, NodeList *prev_list) {

  capture_counter_++;
  reading_counter_++;

  single_c_++;

  Node* new_node = Evaluator::memory_manager_.alloc(t->capture_->S,
                                                  i+1,
                                                  prev_list->head,
                                                  prev_list->tail);
  if(t->capture_->next->visited <= i+1) {
    t->capture_->next->visited = i+2;
    t->capture_->next->currentL->resetAndAdd(new_node);
    new_states_.push_back(t->capture_->next);
  } else {
    t->capture_->next->currentL->add(new_node);
  }
}

FORCE_INLINE void Evaluator::visitDirectSingleCapture(int64_t i, Transition *t, NodeList *prev_list) {

 if(t->direct_->visited <= i+1) {
    t->direct_->visited = i+2;

    // Lazy copy
    t->direct_->currentL->head = prev_list->head;
    t->direct_->currentL->tail = prev_list->tail;

    new_states_.push_back(t->direct_);
  } else {
    t->direct_->currentL->append(prev_list);
  }

  capture_counter_++;
  reading_counter_++;

  direct_single_c_++;

  Node* new_node = Evaluator::memory_manager_.alloc(t->capture_->S,
                                                  i+1,
                                                  prev_list->head,
                                                  prev_list->tail);
  if(t->capture_->next->visited <= i+1) {
    t->capture_->next->visited = i+2;
    t->capture_->next->currentL->resetAndAdd(new_node);
    new_states_.push_back(t->capture_->next);
  } else {
    t->capture_->next->currentL->add(new_node);
  }
}

FORCE_INLINE void Evaluator::visitMultiCapture(int64_t i, Transition *t, NodeList *prev_list) {
  reading_counter_++;
  multi_c_++;
  for(auto &capture: t->captures_) {

    capture_counter_++;

    Node* new_node = Evaluator::memory_manager_.alloc(capture->S,
                                                      i+1,
                                                      prev_list->head,
                                                      prev_list->tail);
    if(capture->next->visited <= i+1) {
      capture->next->visited = i+2;
      capture->next->currentL->resetAndAdd(new_node);
      new_states_.push_back(capture->next);
    } else {
      capture->next->currentL->add(new_node);
    }
  }
}

FORCE_INLINE void Evaluator::visitDirectMultiCapture(int64_t i, Transition *t, NodeList *prev_list) {
  reading_counter_++;
  direct_multi_c_++;
  if(t->direct_->visited <= i+1) {
    t->direct_->visited = i+2;

    // Lazy copy
    t->direct_->currentL->head = prev_list->head;
    t->direct_->currentL->tail = prev_list->tail;

    new_states_.push_back(t->direct_);
  } else {
    t->direct_->currentL->append(prev_list);
  }

  for(auto &capture: t->captures_) {

    capture_counter_++;

    Node* new_node = Evaluator::memory_manager_.alloc(capture->S,
                                                      i+1,
                                                      prev_list->head,
                                                      prev_list->tail);
    if(capture->next->visited <= i+1) {
      capture->next->visited = i+2;
      capture->next->currentL->resetAndAdd(new_node);
      new_states_.push_back(capture->next);
    } else {
      capture->next->currentL->add(new_node);
    }
  }
}

} // end namespace rematch