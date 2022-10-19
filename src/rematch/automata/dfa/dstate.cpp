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