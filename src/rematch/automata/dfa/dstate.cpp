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
    if (p->accepting())
      flags_ |= kAcceptingState;
  }

  update_label();
}

DFA::State::State(size_t tot_states, std::set<LogicalVA::State*> states)
    : id_(ID++), states_bitmap_(tot_states, false),
      states_subset_(states.begin(), states.end()) {
  for (auto &p : states_subset_) {
    states_bitmap_[p->id] = true;
    if (p->accepting())
      flags_ |= kAcceptingState;
  }
  update_label();
}

void DFA::State::add_direct(char a, State *q) {
  if (transitions_[a] == nullptr)
    transitions_[a] = new Transition<State>(q);
  else
    transitions_[a]->add_direct(q);
}

void DFA::State::add_capture(char a, std::bitset<32> S, State *q) {
  if (transitions_[a] == nullptr)
    transitions_[a] = new Transition<State>(S, q);
  else
    transitions_[a]->add_capture({S, q});
}

void DFA::State::add_empty(char a) { transitions_[a] = new Transition<State>(); }

void DFA::State::set_accepting(bool b) {
  if (b)
    flags_ |= Flags::kAcceptingState;
  else
    flags_ &= ~Flags::kAcceptingState;
}

void DFA::State::set_initial(bool b) {
  if (b)
    flags_ |= Flags::kInitialState;
  else
    flags_ &= ~Flags::kInitialState;
}

void DFA::State::pass_node(internal::ECS::Node *n) {
  node = n;
}

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