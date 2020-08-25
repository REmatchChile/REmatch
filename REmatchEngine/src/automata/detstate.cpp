#include <string>
#include "detstate.hpp"

#include <vector>
#include <cassert>
#include <map>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "structures.hpp"
#include "captures.hpp"
#include "det/setstate.hpp"
#include "bitsetwrapper.hpp"
#include "automata/transition.hpp"

using namespace rematch;

unsigned int DetState :: ID = 0;

/****** CONSTRUCTORS ********/

DetState :: DetState()
  : visited(0),
    currentL(new NodeList()),
    copiedList(new NodeList()),
    isFinal(false) {
  id = ID++;
  label = "{0}";
}

DetState :: DetState(SetState* ss)
  : visited(0),
    currentL(new NodeList()),
    copiedList(new NodeList()),
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
    transitions_[a] = new Transition(std::make_unique<Capture>(S, state));
  } else {
    transitions_[a]->add_capture(std::make_unique<Capture>(S, state));
  }
}

void DetState::add_direct(char a, DetState* state) {
  if(transitions_[a] == nullptr) {
    transitions_[a] = new Transition(state);
  } else {
    transitions_[a]->add_direct(state);
  }
}

void DetState::add_empty(char a, DetState* state) {
  transitions_[a] = new Transition();
}


Transition* DetState::next_transition(char a) {
  a &= 0x7F;
  return transitions_[a];
}

std::ostream & operator<<(std::ostream &os, DetState const &q) {
  return os << *(q.ss);
}

