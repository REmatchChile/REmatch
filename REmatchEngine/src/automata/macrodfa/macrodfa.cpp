#include "macrodfa.hpp"

MacroState* MacroDFA::add_state(DetState *state) {
  auto ns = states_.emplace_back(std::make_shared<MacroState>(state));
  return ns.get();
}
MacroState* MacroDFA::add_state(std::vector<DetState*> states) {
  auto ns = states_.emplace_back(std::make_shared<MacroState>(states));
  return ns.get();
}

void MacroDFA::set_as_init(MacroState *ms) {init_state_ = ms;}

MacroState& MacroDFA::get_init_state() {
  return *init_state_;
}