#include <string>
#include "detstate.hpp"

#include <vector>
#include <cassert>
#include <map>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "structs/dag/nodelist.hpp"
#include "captures.hpp"
#include "det/setstate.hpp"
#include "bitsetwrapper.hpp"
#include "automata/dfa/transition.hpp"

namespace rematch {

unsigned int DetState :: ID = 0;

/****** CONSTRUCTORS ********/

DetState::DetState()
    : label("{0}"),
      visited(0),
      currentL(new internal::NodeList()),
      copiedList(new internal::NodeList()),
      isFinal(false) {
  id = ID++;
}

DetState :: DetState(SetState* ss)
  : visited(0),
    currentL(new internal::NodeList()),
    copiedList(new internal::NodeList()),
    ss(ss) {
  id = ID++;
  isFinal = ss->isFinal;
  isSuperFinal = ss->isSuperFinal;
  mark = false;
  std::stringstream s;
  s << *(ss);
  label = s.str();
}

void DetState :: setFinal(bool b) {isFinal = b;}

void DetState :: setSubset(SetState* newss) {
  ss = newss;
  isFinal = ss->isFinal;
  isSuperFinal = ss->isSuperFinal;
}


void DetState::add_capture(char a, std::bitset<32> S, DetState* state) {
  if(transitions_[a] == nullptr) {
    transitions_[a] = std::make_unique<Transition>(std::make_unique<Capture>(S, state));
  } else {
    transitions_[a]->add_capture(std::make_unique<Capture>(S, state));
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

} // end namespace rematch