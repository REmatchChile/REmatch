#include "macrostate.hpp"

#include "automata/macrodfa/macrotransition.hpp"

MacroState::MacroState(DetState* state) {
  states_.push_back(state);
};

MacroState::MacroState(std::vector<DetState*> states) : states_(states) {}

MacroTransition* MacroState::next_transition(char a) {
  return transitions_[a].get();
}

void MacroState::add_transition(char a, std::shared_ptr<MacroTransition> tr) {
  transitions_[a] = tr;
}

std::vector<DetState*>& MacroState::states() {
  return states_;
}

bool MacroState::is_super_final() const {
  for(auto &state: states_) {
    if(state->isSuperFinal)
      return true;
  }
  return false;
}


