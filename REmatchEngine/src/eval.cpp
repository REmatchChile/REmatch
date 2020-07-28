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
      nlines_(0) {
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
      nlines_(0) {
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

void Evaluator::initAutomaton(size_t i) {
  DFA().initState()->visited = i+1;
  if( i == 0)
    DFA().initState()->currentL->add(Evaluator::memory_manager_.alloc());

  current_states_.clear();
  current_states_.push_back(DFA().initState());

  capture_states_.clear();
  if(!DFA().initState()->c.empty())
      capture_states_.push_back(DFA().initState());

  if(early_output_) captureT(i);
  else              captureF(i);
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

    while(((i_pos_-i_start_) < line_.size() &&  line_by_line_) ||
          (i_pos_ < text_->size()             && !line_by_line_)) { // Main search loop

      if(line_by_line_)   a = line_[i_pos_-i_start_];
      else                a = text_->get();

      if(early_output_)   readingT(a, i_pos_);
      else                readingF(a, i_pos_);

      current_states_.swap(new_states_);

      if(early_output_)   captureT(i_pos_+1);
      else                captureF(i_pos_+1);

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

    if(((i_pos_-i_start_) == line_.size()   &&  line_by_line_) ||
       (i_pos_ == text_->size()             && !line_by_line_)) {
      if(line_by_line_) {
        while(!(document_ended_ = !((bool) text_->getline(line_)))) {
          line_ += '\n';
          i_pos_++;
          i_start_ = i_pos_;

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

  size_t it = 0;

  while( it < line_.size() ) {
    a = (char) line_[it];
    // nextState is reached from currentState by reading the character
    nextState = currentState->nextState(a);

    if(nextState == nullptr) // Then maybe a determinization is needed
      nextState = rgx_.rawDetManager().getNextDetState(currentState, a);

    if (nextState->isSuperFinal)
      return true;

    if (nextState->ss->isNonEmpty)  // Check if not empty set
      currentState = nextState;
    else
      return false;
    it++;
  }

  return currentState->isFinal;
}

inline void Evaluator::capture(size_t i, bool early_output) {
  DetState* nextState;
  Node* newNode;

  for (auto &currentState: capture_states_) {
    currentState->copiedList->head = currentState->currentL->head;
    currentState->copiedList->tail = currentState->currentL->tail;
  }

  for (auto &currentState: capture_states_) {
    for (auto &capture: currentState->c) {

      nextState = capture->next;

      newNode = Evaluator::memory_manager_.alloc(capture->S, i,
                                       currentState->copiedList->head,
                                       currentState->copiedList->tail);
      // Early output case
      if(early_output && nextState->isSuperFinal)
        output_nodelist_.add(newNode);
      else {
        if (nextState->visited < i+1) {
          nextState->currentL->resetAndAdd(newNode);
          nextState->visited = i+1;

          current_states_.push_back(nextState);
        }
        else {
          nextState->currentL->add(newNode);
        }
      }
    }
  }
}

inline void Evaluator::reading(char a, size_t i, bool early_output) {
  capture_states_.clear();
  new_states_.clear();
  DetState* nextState;
  NodeList* prevList;

  for (auto &currentState: current_states_) {

#ifdef NOPT_CROSSPROD
    if(currentState->visited == i+2)
      prevList = currentState->oldL;
    else
#endif
      prevList = currentState->currentL;

    // nextState is reached from currentState by reading the character
    nextState = currentState->nextState(a);

    if(nextState == nullptr) { // Then maybe a determinization is needed
      nextState = rgx_.detManager().getNextDetState(currentState, a);
    }

    if(early_output && nextState->isSuperFinal) {  // Early Output check
      output_nodelist_.append(prevList);
    }
    else {
      if (nextState->ss->isNonEmpty) {  // Check if not empty set
        if (nextState->visited <= i+1) { // If not already visited
          nextState->visited = i+2; // Mark as visited

#ifdef NOPT_CROSSPROD
          nextState->oldL->head = nextState->currentL->head;
          nextState->oldL->tail = nextState->currentL->tail;
#endif

          // Pass the list to nextState
          nextState->currentL->head = prevList->head;
          nextState->currentL->tail = prevList->tail;

          this->new_states_.push_back(nextState);

          if (!nextState->c.empty())
            this->capture_states_.push_back(nextState);

        }
        else { // If already visited
          // Append (concat) nextState's currentL with previous list
          nextState->currentL->append(prevList);
        }
      }
      else { // If empty set is reached then consider adding to garbage collection
        prevList->resetRefs();
        Evaluator::memory_manager_.addPossibleGarbage(prevList->head);
      }
    }
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
void Evaluator::captureT(size_t i) {
  capture(i, 1);
}
void Evaluator::captureF(size_t i) {
  capture(i, 0);
}
void Evaluator::readingT(char a, size_t i) {
  reading(a, i, 1);
}
void Evaluator::readingF(char a, size_t i) {
  reading(a, i, 0);
}

} // end namespace rematch