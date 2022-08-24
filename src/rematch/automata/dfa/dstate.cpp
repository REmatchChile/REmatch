#include "dstate.hpp"

#include <algorithm>

#include "automata/nfa/state.hpp"

namespace rematch {

int DFA::State::ID = 0;


DFA::State::State(std::vector<LogicalVA::State*> states)
    : id_(ID++), states_subset_(states) {
  for (auto &p : states_subset_) {
    set_accepting(p->accepting());
  }
  #ifndef NDEBUG
  update_label();
  #endif
}

void DFA::State::add_direct(char a, State *q) {
  if (!transitions_[a])
    transitions_[a] = Transition(q);
  else
    transitions_[a]->add_direct(q);
}

void DFA::State::add_capture(char a, std::bitset<32> S, State *q) {
  if (!transitions_[a])
    transitions_[a] = Transition(S, q);
  else
    transitions_[a]->add_capture({S, q});
}

void DFA::State::add_empty(char a) { transitions_[a] = Transition(); }

#ifndef NDEBUG
void DFA::State::update_label() {
  std::stringstream ss;
  ss << "{ ";
  for (auto p : states_subset_) {
    ss << p->id << ' ';
  }
  ss << '}';
  label_ = ss.str();
}
#endif

} // end namespace rematch