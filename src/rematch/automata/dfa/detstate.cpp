#include <string>
#include "detstate.hpp"

#include <vector>
#include <cassert>
#include <map>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "structs/dag/fastnodelist.hpp"
#include "captures.hpp"
#include "det/setstate.hpp"
#include "bitsetwrapper.hpp"
#include "automata/dfa/transition.hpp"
#include "automata/nfa/state.hpp"

namespace rematch {

unsigned int DetState :: ID = 0;

/****** CONSTRUCTORS ********/

DetState::DetState()
    : label("{0}"),
      visited(-1),
      currentL(new internal::FastNodeList()),
      copiedList(new internal::FastNodeList()),
      isFinal(false) {
  id = ID++;
}

DetState :: DetState(SetState* ss)
  : visited(-1),
    currentL(new internal::FastNodeList()),
    copiedList(new internal::FastNodeList()),
    ss(ss) {
  id = ID++;
  setSubset(ss);
}

void DetState :: setFinal(bool b) {isFinal = b;}

void DetState :: setSubset(SetState* newss) {
  ss = newss;
  isFinal = ss->isFinal;
  isSuperFinal = ss->isSuperFinal;
  isOnlyInit = true;
  hasCapture = false;
  for(auto &state: ss->subset) {
    if(!(state->flags() & State::kInitialState))
      isOnlyInit = false;
    if(state->flags() & State::kCaptureState)
      hasCapture = true;
  }
  mark = false;
  std::stringstream s;
  s << *(ss);
  label = s.str();
}


void DetState::add_capture(char a, std::bitset<32> S, DetState* state) {
  if(transitions_[a] == nullptr) {
    transitions_[a] = std::make_unique<Transition>(new Capture(S, state));
  } else {
    transitions_[a]->add_capture(new Capture(S, state));
  }
}

void DetState::add_direct(char a, DetState* state) {
  if(transitions_[a] == nullptr) {
    transitions_[a] = std::make_unique<Transition>(state);
  } else {
    transitions_[a]->add_direct(state);
  }
}

void DetState::add_empty(char a, DetState* state) {
  transitions_[a] = std::make_unique<Transition>();
}


Transition* DetState::next_transition(char a) {
  return transitions_[a].get();
}

std::ostream & operator<<(std::ostream &os, DetState const &q) {
  return os << *(q.ss);
}


bool DetState::remove_superfinals() {
  auto old_size = ss->subset.size();
  if(isSuperFinal) {
    bool final_check = false;
    for(auto it = ss->subset.begin(); it != ss->subset.end();) {
      if((*it)->isSuperFinal) {
        it = ss->subset.erase(it);
      } else {
        if((*it)->isFinal)
          final_check = true;
        ++it;
      }
    }
    ss->isFinal = final_check;
    ss->isSuperFinal = false;
    setSubset(ss);
  }

  return old_size > ss->subset.size();
}

bool DetState::empty() const {
  return ss->subset.empty();
}

void DetState::pass_node(internal::ECS::Node* node) {
  currentNode = node;
  ++node->ref_count_;
}

} // end namespace rematch