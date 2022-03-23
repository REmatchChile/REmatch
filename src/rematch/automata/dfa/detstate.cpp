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
      copiedList(new internal::FastNodeList())  {
  id = ID++;
}

DetState::DetState(SetState* ss)
  : visited(-1),
    currentL(new internal::FastNodeList()),
    copiedList(new internal::FastNodeList()),
    ss(ss) {
  id = ID++;
  set_subset(ss);
}

void DetState::set_initial(bool b) {
  if(b)
    flags_ |= Flags::kInitial;
  else
    flags_ &= ~Flags::kInitial;
}

void DetState::set_accepting(bool b) {
  if(b)
    flags_ |= Flags::kAccepting;
  else
    flags_ &= ~Flags::kAccepting;
}

void DetState::set_subset(SetState* newss) {
  ss = newss;
  set_accepting(ss->isFinal);
  set_initial(true);
  for(auto &state: ss->subset) {
    if(!(state->flags() & State::kInitialState))
      set_initial(false);
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


bool DetState::empty() const {
  return ss->subset.empty();
}

void DetState::pass_node(internal::ECS::Node* node) {
  currentNode = node;
  ++node->ref_count_;
}

} // end namespace rematch