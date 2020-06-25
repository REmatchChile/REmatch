#include "eval.hpp"

#include "automata/detstate.hpp"

namespace rematch {

Evaluator::Evaluator(RegEx &rgx, std::istream& input,
                     uint8_t flags)
    : rgx_(rgx),
      input_stream_(&input),
      early_output_(flags & kEarlyOutput),
      line_by_line_(flags & kLineByLine),
      document_ended_(false),
      i_pos_(0),
      i_start_(0),
      nlines_(0) {
  init();
}

Evaluator::Evaluator(RegEx &rgx, const std::string &text,
                     uint8_t flags)
    : rgx_(rgx),
      input_stream_(new std::istringstream(text)),
      early_output_(flags & kEarlyOutput),
      line_by_line_(flags & kLineByLine),
      document_ended_(false),
      i_pos_(0),
      i_start_(0),
      nlines_(0) {
  init();
}

void Evaluator::init() {
  enumerator_ = std::make_unique<Enumerator>(rgx_, text_, memory_manager_);
  if(line_by_line_) {
    std::getline(*input_stream_, text_);
    text_ += '\n';
    nlines_++;
  }
  else {
    input_stream_->seekg(0, input_stream_->end);
    text_.resize(input_stream_->tellg());
    input_stream_->seekg(0, input_stream_->beg);
    input_stream_->read(&text_[0], text_.size());
  }
  initAutomaton(i_pos_);
}

void Evaluator::initAutomaton(size_t i) {
  DFA().initState()->visited = i;
  if( i == 0)
    DFA().initState()->currentL->add(memory_manager_.alloc());

  current_states_.clear();
  current_states_.push_back(DFA().initState());

  capture_states_.clear();
  if(!DFA().initState()->c.empty())
      capture_states_.push_back(DFA().initState());

  if(early_output_) captureT(i);
  else              captureF(i);
}

std::unique_ptr<Match> Evaluator::next() {
  // Decide which next to use.
  static std::unique_ptr<Match> (Evaluator::*nexts[])() = {
    &Evaluator::nextFF,
    &Evaluator::nextFT,
    &Evaluator::nextTF,
    &Evaluator::nextTT,
  };
  int index = 2 * line_by_line_ +
              1 * early_output_;

  return (this->*nexts[index])();
}

inline std::unique_ptr<Match>
Evaluator::inlinedNext(bool early_output, bool line_by_line) {

  if(enumerator_->hasNext())
      return enumerator_->next();

  while (!document_ended_) {

    char a;
    output_nodelist_.reset();

    while((i_pos_-i_start_) < text_.size()) { // Main search loop
      a = (char) text_[i_pos_-i_start_];

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
    if(! output_nodelist_.empty())
      enumerator_->addNodeList(output_nodelist_);

    if((i_pos_-i_start_) == text_.size()) {
      if(line_by_line_) {
        while(!(document_ended_ = !((bool) std::getline(*input_stream_, text_)))) {
          text_ += '\n';
          i_pos_++;
          i_start_ = i_pos_;

          if(!match()) {
            i_pos_ += text_.size() - 1;
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

  while( it < text_.size() ) {
    // std::cout << "it = " << it << "size = " << text_.size() << '\n';
    a = (char) text_[it];
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

      newNode = memory_manager_.alloc(capture->S, i,
                                       currentState->copiedList->head,
                                       currentState->copiedList->tail);
      // Early output case
      if(early_output && nextState->isSuperFinal)
        output_nodelist_.add(newNode);
      else {
        if (nextState->visited <= i) {
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

  for (auto &currentState: current_states_) {

    // nextState is reached from currentState by reading the character
    nextState = currentState->nextState(a);

    if(nextState == nullptr) { // Then maybe a determinization is needed
      nextState = rgx_.detManager().getNextDetState(currentState, a);
    }

    if(early_output && nextState->isSuperFinal) {  // Early Output check
      output_nodelist_.append(currentState->currentL);
    }
    else {
      if (nextState->ss->isNonEmpty) {  // Check if not empty set
        if (nextState->visited <= i) { // If not already visited
          nextState->visited = i+1; // Mark as visited

          // Pass the list to nextState
          nextState->currentL->head = currentState->currentL->head;
          nextState->currentL->tail = currentState->currentL->tail;

          this->new_states_.push_back(nextState);

          if (!nextState->c.empty())
            this->capture_states_.push_back(nextState);

        }
        else { // If already visited
          // Append (concat) nextState's currentL with previous list
          nextState->currentL->append(currentState->currentL);
        }
      }
      else { // If empty set is reached then consider adding to garbage collection
        currentState->currentL->resetRefs();
        memory_manager_.addPossibleGarbage(currentState->currentL->head);
      }
    }
  }
}


// Callers of inline versions

std::unique_ptr<Match> Evaluator::nextFF() {
  return inlinedNext(0, 0);
}
std::unique_ptr<Match> Evaluator::nextFT() {
  return inlinedNext(0, 1);
}
std::unique_ptr<Match> Evaluator::nextTF() {
  return inlinedNext(1, 0);
}
std::unique_ptr<Match> Evaluator::nextTT() {
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