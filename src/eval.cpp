#include "eval.hpp"

#include "automata/dfa/detstate.hpp"
#include "memmanager.hpp"
#include "timer.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

namespace rematch {

MemManager Evaluator::memory_manager_{};

Evaluator::Evaluator(RegEx &rgx, std::shared_ptr<Document> d,
                     uint8_t flags)
    : rgx_(rgx),
      text_(d),
      early_output_(flags & kEarlyOutput),
      line_by_line_(flags & kLineByLine),
      document_ended_(false),
      direct_text_(false),
      i_pos_(0),
      i_start_(0) {
  init();
}

void Evaluator::init() {
  std::string str_ = "";
  enumerator_ = std::make_unique<Enumerator>(rgx_, str_);
  if(line_by_line_) {
    text_->getline(line_);
    // line_ += '\n';
    nlines_++;
  }
  initAutomaton(i_pos_);
}

void Evaluator::initAutomaton(int64_t i) {
  int64_t pos;
  if(i == 0) {
    dfa().initState()->currentL->add(Evaluator::memory_manager_.alloc());
    pos = i-1;
    macro_dfa_.set_as_init(macro_dfa_.add_state(dfa().initState()));
  } else {
    pos = i;
  }

  current_state_ = &macro_dfa_.get_init_state();

  reading(0, pos, 0);
}

Match_ptr Evaluator::next() {
  if(enumerator_->hasNext())
      return enumerator_->next();

  // bool exited = false;

  // Timer t; 

  while (!document_ended_) {
    char a;
    output_nodelist_.reset();

    while((i_pos_-i_start_) < (int64_t)line_.size()) { // Main search loop

      a = line_[i_pos_-i_start_];

      a &= 0x7F;  // Only ASCII chars for now

      reading(a, i_pos_, 0);

      i_pos_++;

      // if(current_state_->is_super_final()) {
      //   exited = true;
      //   break;
      // }
    }

    for(auto &state: current_state_->states()) {
      if(state->isFinal) {
        output_nodelist_.append(state->currentL);
        state->currentL->reset();
      }
    }
    if(!output_nodelist_.empty()) {
      enumerator_->addNodeList(output_nodelist_);
      output_nodelist_.resetRefs();
      Evaluator::memory_manager_.addPossibleGarbage(output_nodelist_.head);
      // if(exited) {
      //   break;
      // }
    }

    if((i_pos_-i_start_) == (int64_t)line_.size()) {
      while(!(document_ended_ = !((bool) text_->getline(line_)))) {
        // line_ += '\n';
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

  // std::cout << "[In code] Eval time: " << t.elapsed() << "\n";

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

void Evaluator::reading(char a, int64_t i_pos,  bool early_output) {
  auto nextTransition = current_state_->next_transition(a);

  if(nextTransition == nullptr) {
    nextTransition = rgx_.detManager().next_macro_transition(current_state_, a);
  }

  auto directs = nextTransition->directs();
  auto captures = nextTransition->captures();

  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  auto directs_sz = nextTransition->ndirects_;
  auto captures_sz = nextTransition->ncaptures_;
  #else
  auto directs_sz = directs.size();
  auto captures_sz = captures.size();
  #endif


  for(size_t i=0; i < directs_sz; i++) {
    auto direct = directs[i];

    if(direct.to->visited <= i_pos+1) {
      direct.to->visited = i_pos+2;
      // Lazy copy
      direct.to->currentL->head = direct.from->currentL->head;
      direct.to->currentL->tail = direct.from->currentL->tail;
    } else {
      direct.to->currentL->append(direct.from->currentL);
    }
  }

  for(size_t i=0; i < captures_sz; i++) {
    auto capture = captures[i];
    #ifndef NOPT_MEMORY_MANAGER
    Node* new_node = Evaluator::memory_manager_.alloc(capture.S,
                                                      i_pos+1,
                                                      capture.from->currentL->head,
                                                      capture.from->currentL->tail);
    #else
    Node* new_node = new Node(capture.s, i_pos_+1, capture.from->currentL->head,
                              capture.from->currentL->tail)
    #endif
    if(capture.to->visited <= i_pos+1) {
      capture.to->visited = i_pos+2;
      capture.to->currentL->resetAndAdd(new_node);
    } else {
      capture.to->currentL->add(new_node);
    }
  }

  current_state_ = nextTransition->next_state();
}

} // end namespace rematch