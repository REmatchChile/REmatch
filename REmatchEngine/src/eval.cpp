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

void Evaluator::initAutomaton(int64_t i) {
  if( i == 0)
    DFA().initState()->currentL->add(Evaluator::memory_manager_.alloc());

  current_states_.clear();
  current_states_.push_back(DFA().initState());

  if(early_output_) readingT(0, i-1);
  else              readingF(0, i-1);

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

    a = (char) line_[it];

    // nextState is reached from currentState by reading the character
    auto &nextCaptures = currentState->next_captures(a);

    if(nextCaptures.empty()) // Then maybe a determinization is needed
      nextCaptures = rgx_.rawDetManager().next_captures(currentState, a);

    // TODO: Remove this once it's working
    assert(nextCaptures.size() == 1);

    nextState = nextCaptures[0]->next;

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


inline void Evaluator::reading(char a, int64_t i, bool early_output) {
  new_states_.clear();
  DetState* nextState;
  NodeList* prevList;
  Node* newNode;

  for (auto &currentState: current_states_) {
    prevList = currentState->currentL;
    // nextCaptures are reached from currentState by reading the character
    auto nextCaptures = currentState->next_captures(a);

    if(nextCaptures.empty()) { // Then maybe a determinization is needed
      nextCaptures = rgx_.detManager().next_captures(currentState, a);
    }

    for(auto &capture: nextCaptures) {
      nextState = capture->next;

      if(early_output && nextState->isSuperFinal) {  // Early Output check
        output_nodelist_.append(prevList);
      } else {
        if (!nextState->ss->isNonEmpty) {  // Check if not empty set
          prevList->resetRefs();
          Evaluator::memory_manager_.addPossibleGarbage(prevList->head);
        } else {
          if(capture->S.none()) {
            if(nextState->visited <= i+1) {
              nextState->visited = i+2;
              nextState->currentL->head = prevList->head;
              nextState->currentL->tail = prevList->tail;
              new_states_.push_back(nextState);
            } else {
              nextState->currentL->append(prevList);
            }
          } else {
            newNode = Evaluator::memory_manager_.alloc(capture->S, i+1,
                                              currentState->currentL->head,
                                              currentState->currentL->tail);
            if(nextState->visited <= i+1) {
              nextState->currentL->resetAndAdd(newNode);
              nextState->visited = i+2;
              new_states_.push_back(nextState);
            } else {
              nextState->currentL->add(newNode);
            }
          }
        }
      }
    } // for(capture : nextCaptures)
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

} // end namespace rematch