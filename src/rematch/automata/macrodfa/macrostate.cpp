#include "macrostate.hpp"

#include "automata/macrodfa/macrotransition.hpp"
#include "automata/dfa/dstate.hpp"

namespace rematch {

MacroState::MacroState(DState* state) {
  states_.push_back(state);
}

MacroState::MacroState(std::vector<DState*> states) : states_(states) {}

MacroTransition* MacroState::next_transition(char a) {
  return transitions_[a].get();
}

void MacroState::add_transition(char a, std::shared_ptr<MacroTransition> tr) {
  transitions_[a] = tr;
}

std::vector<DState*>& MacroState::states() {
  return states_;
}

// void MacroState::remove_super_finals() {
//   for(auto it=states_.begin(); it != states_.end();) {
//     (*it)->remove_superfinals();
//     if((*it)->ss->subset.empty()) {
//       it = states_.erase(it);
//     } else {
//       ++it;
//     }
//   }
// }

} // end namespace rematch