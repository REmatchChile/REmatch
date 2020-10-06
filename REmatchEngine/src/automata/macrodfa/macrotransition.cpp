#include "macrotransition.hpp"

MacroTransition::MacroTransition(char a): label_(a) {}

void MacroTransition::add_direct(char a, DetState& from, DetState& to) {
  directs_.emplace_back(from, to);
}

void MacroTransition::add_capture(char a, DetState& from, std::bitset<32> S,
                                  DetState& to) {
  captures_.emplace_back(from, S, to);
}

std::vector<MTDirect>& MacroTransition::directs() {return directs_;}
std::vector<MTCapture>& MacroTransition::captures() {return captures_;}

MacroState* MacroTransition::next_state() {return next_.get();}