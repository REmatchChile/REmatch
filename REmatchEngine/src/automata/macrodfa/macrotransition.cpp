#include "macrotransition.hpp"

MacroTransition::MacroTransition() = default;

void MacroTransition::add_direct(DetState& from, DetState& to) {
  directs_.emplace_back(from, to);
}

void MacroTransition::add_capture(DetState& from, std::bitset<32> S,
                                  DetState& to) {
  captures_.emplace_back(from, S, to);
}

void MacroTransition::set_next_state(MacroState* ms) {next_ = ms;}

std::vector<MTDirect>& MacroTransition::directs() {return directs_;}
std::vector<MTCapture>& MacroTransition::captures() {return captures_;}

MacroState* MacroTransition::next_state() {return next_;}