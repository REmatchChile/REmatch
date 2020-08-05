#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <sstream>
#include <unordered_map>

#include "detstate.hpp"
#include "structures.hpp"
#include "captures.hpp"
#include "det/setstate.hpp"
#include "bitsetwrapper.hpp"

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


void DetState::add_transition(char a, std::bitset<32> S, DetState* state) {
  if(S.count())
    capture_indices_.push_back(a);
  transitions_[a].emplace_back(new Capture(S, state));
}


std::vector<Capture*> & DetState::next_captures(char a) {
  a &= 0x7F;
  return transitions_[a];
}

std::ostream & operator<<(std::ostream &os, DetState const &q) {
  return os << *(q.ss);
}

