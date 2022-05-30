#include "macrostate.hpp"

#include "automata/dfa/dstate.hpp"
#include "automata/macrodfa/macrotransition.hpp"

namespace rematch {

int MacroState::current_ID = 0;

MacroState::MacroState(DState *state) { states_.push_back(state); }

MacroState::MacroState(std::vector<DState *> states) : states_(states) {}

MacroTransition *MacroState::next_transition(char a) {
  return transitions_[a].get();
}

void MacroState::add_transition(char a, std::shared_ptr<MacroTransition> tr) {
  transitions_[a] = tr;
}

std::vector<DState *> &MacroState::states() { return states_; }

} // end namespace rematch