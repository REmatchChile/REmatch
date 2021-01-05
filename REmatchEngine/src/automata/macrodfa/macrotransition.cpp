#include "macrotransition.hpp"

MacroTransition::MacroTransition()
  : directs_idx_{0}, captures_idx_{0} {
    // directs_.reserve(10);
    // captures_.reserve(10);
  }

void MacroTransition::add_direct(DetState& from, DetState& to) {
  // directs_.emplace_back(from, to);
  new (&directs_[directs_idx_++]) MTDirect(from, to);
}

void MacroTransition::add_capture(DetState& from, std::bitset<32> S,
                                  DetState& to) {
  // captures_.emplace_back(from, S, to);
  new (&captures_[captures_idx_++]) MTCapture(from, S, to);
}

void MacroTransition::set_next_state(MacroState* ms) {next_ = ms;}

MTDirect* MacroTransition::directs() {return directs_;}
MTCapture* MacroTransition::captures() {return captures_;}

MacroState* MacroTransition::next_state() {return next_;}