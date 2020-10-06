#include "macrodfa.hpp"

MacroState& MacroDFA::add_state(DetState *state) {
  return states_.emplace_back(state); // C++17
}
MacroState& MacroDFA::add_state(std::vector<DetState*> states) {
  return states_.emplace_back(states); // C++17
}

void MacroDFA::set_as_init(MacroState &ms) {init_state_ = &ms;}

void MacroDFA::set_as_final(MacroState &ms) {
  final_states_.push_back(std::ref(ms));
}

MacroState& MacroDFA::get_init_state() {
  return *init_state_;
}