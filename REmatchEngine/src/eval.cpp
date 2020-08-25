#include "eval.hpp"

#include "automata/detstate.hpp"
#include "memmanager.hpp"

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
      reading_counter_(0) {
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
      reading_counter_(0) {
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

  if(early_output_) readingT(0, pos);
  else              readingF(0, pos);

  current_states_.swap(new_states_);
}

Match_ptr Evaluator::next() {
  // Decide which next to use.
  static Match_ptr (Evaluator::*nexts[])() = {
    &Evaluator::nextFF,
    &Evaluator::nextFT,
    &Evaluator::nextTF,
    &Evaluator::nextTT,
  };
  int index = 2 * line_by_line_ +
              1 * early_output_;

  return (this->*nexts[index])();
}

inline Match_ptr
Evaluator::inlinedNext(bool early_output, bool line_by_line) {

  if(enumerator_->hasNext())
      return enumerator_->next();

  while (!document_ended_) {
    char a;
    output_nodelist_.reset();

    while(((i_pos_-i_start_) < (int64_t)line_.size() &&  line_by_line_) ||
          (i_pos_ < (int64_t)text_->size()           && !line_by_line_)) { // Main search loop

      if(line_by_line_)   a = line_[i_pos_-i_start_];
      else                text_->get(a);

      a &= 0x7F;

      if(early_output_)   readingT(a, i_pos_);
      else                readingF(a, i_pos_);

      current_states_.swap(new_states_);

      i_pos_++;

      if(early_output_) {
        if(!output_nodelist_.empty())
          break;
      }
    }

    for(auto &state: current_states_) {
      if(state->isFinal)
        output_nodelist_.append(state->currentL);
    }
    if(!output_nodelist_.empty()) {
      enumerator_->addNodeList(output_nodelist_);
      Evaluator::memory_manager_.addPossibleGarbage(output_nodelist_.head);
    }

    if(((i_pos_-i_start_) == (int64_t)line_.size()   &&  line_by_line_) ||
       (i_pos_ == (int64_t)text_->size()             && !line_by_line_)) {
      if(line_by_line_) {
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
      else {
        document_ended_ = true;
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

    if (nextTransition->type_ == Transition::Type::kEmpty)
      return false;

    nextState = nextTransition->direct_;

    if (nextState->isSuperFinal)
      return true;

    currentState = nextState;
    it++;
  }

  return currentState->isFinal;
}


inline void Evaluator::reading(char a, int64_t i, bool early_output) {
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
    }

    (this->*visits[(int)nextTransition->type_])(i, nextTransition, prevList);
  }
}

// Callers of inline versions

Match_ptr Evaluator::nextFF() {
  return inlinedNext(0, 0);
}
Match_ptr Evaluator::nextFT() {
  return inlinedNext(0, 1);
}
Match_ptr Evaluator::nextTF() {
  return inlinedNext(1, 0);
}
Match_ptr Evaluator::nextTT() {
  return inlinedNext(1, 1);
}
void Evaluator::readingT(char a, int64_t i) {
  reading(a, i, 1);
}
void Evaluator::readingF(char a, int64_t i) {
  reading(a, i, 0);
}

inline void Evaluator::visitEmpty(int64_t i, Transition* t, NodeList *prev_list) {
  prev_list->resetRefs();
  Evaluator::memory_manager_.addPossibleGarbage(prev_list->head);
}

inline void Evaluator::visitDirect(int64_t i, Transition* t, NodeList *prev_list) {

  reading_counter_++;

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

inline void Evaluator::visitSingleCapture(int64_t i, Transition* t, NodeList *prev_list) {

  capture_counter_++;
  reading_counter_++;

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

inline void Evaluator::visitDirectSingleCapture(int64_t i, Transition* t, NodeList *prev_list) {

  reading_counter_++;

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

inline void Evaluator::visitMultiCapture(int64_t i, Transition* t, NodeList *prev_list) {
  for(auto &capture: t->captures_) {

    capture_counter_++;
    reading_counter_++;

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

inline void Evaluator::visitDirectMultiCapture(int64_t i, Transition* t, NodeList *prev_list) {
  reading_counter_++;
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
    reading_counter_++;

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