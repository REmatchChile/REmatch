#include "macrodfa.hpp"

#include <memory>

#include "automata/macrodfa/macrostate.hpp"

namespace rematch {

MacroState* MacroDFA::add_state(DetState *state) {
  states_.emplace_back(std::make_shared<MacroState>(state));
  return states_.back().get();
}
MacroState* MacroDFA::add_state(std::vector<DetState*> states) {
  states_.emplace_back(std::make_shared<MacroState>(states));
  return states_.back().get();
}

void MacroDFA::set_as_init(MacroState *ms) {init_state_ = ms;}

MacroState& MacroDFA::get_init_state() {
  return *init_state_;
}

} // end namespace rematch