#include "dstate.hpp"

#include <algorithm>

#include "automata/nfa/state.hpp"

namespace rematch {

int DFA::State::ID = 0;

DFA::State::State(size_t tot_states)
    : id_(ID++), states_bitmap_(tot_states, false) {}

DFA::State::State(size_t tot_states, std::vector<LogicalVA::State*> states)
    : id_(ID++), states_bitmap_(tot_states, false), states_subset_(states) {
  for (auto &p : states_subset_) {
    states_bitmap_[p->id] = true;
    set_accepting(p->accepting());
  }

  update_label();
}

DFA::State::State(size_t tot_states, std::set<LogicalVA::State*> states)
    : id_(ID++), states_bitmap_(tot_states, false),
      states_subset_(states.begin(), states.end()) {
  for (auto &p : states_subset_) {
    states_bitmap_[p->id] = true;
    set_accepting(p->accepting());
  }
  update_label();
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

void DFA::State::update_label() {
  std::stringstream ss;
  ss << "{ ";
  for (auto p : states_subset_) {
    ss << p->id << ' ';
  }
  ss << '}';
  labl = ss.str();
}

} // end namespace rematch