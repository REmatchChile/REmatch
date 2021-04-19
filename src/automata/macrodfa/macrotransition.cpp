#include "macrotransition.hpp"

namespace rematch {

MacroTransition::MacroTransition()
  : ndirects_{0}, ncaptures_{0}, nempties_{0} {
    // directs_.reserve(10);
    // captures_.reserve(10);
  }

MacroTransition::MacroTransition(size_t ndirects, size_t ncaptures, size_t nempties)
  : ndirects_{0}, ncaptures_{0}, nempties_{0},
    directs_{new MTDirect[ndirects]},
    captures_{new MTCapture[ncaptures]},
    empties_{new DetState*[nempties]} {}

void MacroTransition::add_direct(DetState& from, DetState& to) {
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  new (&directs_[ndirects_++]) MTDirect(from, to);
  #else
  directs_.emplace_back(from, to);
  #endif
}

void MacroTransition::add_capture(DetState& from, std::bitset<32> S,
                                  DetState& to) {
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  new (&captures_[ncaptures_++]) MTCapture(from, S, to);
  #else
  captures_.emplace_back(from, S, to);
  #endif
}

void MacroTransition::add_empty(DetState& from) {
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  empties_[nempties_++] = &from;
  #else
  captures_.emplace_back(from, S, to);
  #endif
}

void MacroTransition::set_next_state(MacroState* ms) {next_ = ms;}

DirectsArray MacroTransition::directs() {return directs_;}
CapturesArray MacroTransition::captures() {return captures_;}
EmptiesArray MacroTransition::empties() {return empties_;}

MacroState* MacroTransition::next_state() {return next_;}

} // end namespace rematch