#include "macrostate.hpp"

#include "automata/macrodfa/macrotransition.hpp"

MacroState::MacroState(DetState* state) {
  states_.push_back(state);
};

MacroState::MacroState(std::vector<DetState*> states) : states_(states) {}

MacroTransition* MacroState::next_transition(char a) {
  auto ret = transitions_.find(a);
  if(ret != transitions_.end()) {
    return ret->second.get();
  }
  return nullptr;
}

void MacroState::add_transition(char a, std::shared_ptr<MacroTransition> tr) {
  transitions_[a] = tr;
}

std::vector<DetState*>& MacroState::states() {
  return states_;
}