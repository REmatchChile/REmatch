#include "macrostate.hpp"

#include "automata/macrodfa/macrotransition.hpp"

MacroState::MacroState(DetState* state) {
  states_.push_back(state);
};

MacroState::MacroState(std::vector<DetState*> states) : states_(states) {}

MacroTransition* MacroState::next_transition(char a) {
  auto ret = transitions_.find(a);
  if(ret != transitions_.end()) {
    return &ret->second;
  }
  return nullptr;
}

void MacroState::add_transition(char a){
  transitions_.emplace(a, MacroTransition(a));
}

std::vector<DetState*>& MacroState::states() {
  return states_;
}