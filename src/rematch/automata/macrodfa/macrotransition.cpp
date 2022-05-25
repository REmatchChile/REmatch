#include "macrotransition.hpp"

namespace rematch {

MacroTransition::MacroTransition(size_t nfirstdirects, size_t nrepeatdirects,
                                 size_t nfirstcaptures, size_t nrepeatcaptures,
                                 size_t nempties)
    : first_directs_{new MTDirect[nfirstdirects]},
      repeat_directs_{new MTDirect[nrepeatdirects]},
      first_captures_{new MTCapture[nfirstcaptures]},
      repeat_captures_{new MTCapture[nrepeatcaptures]},
      empties_{new DState*[nempties]} {}

void MacroTransition::add_direct(DState& from, DState& to, bool first) {
  if (first)
    new (&first_directs_[nfirstdirects_++]) MTDirect(from, to);
  else
    new (&repeat_directs_[nrepeatdirects_++]) MTDirect(from, to);
}

void MacroTransition::add_capture(DState& from, std::bitset<32> S,
                                  DState& to, bool first) {
  if (first)
    new (&first_captures_[nfirstcaptures_++]) MTCapture(from, S, to);
  else
    new (&repeat_captures_[nrepeatcaptures_++]) MTCapture(from, S, to);
}

void MacroTransition::add_empty(DState& from) {
  empties_[nempties_++] = &from;
}


void MacroTransition::set_next_state(MacroState* ms) {next_ = ms;}

DirectsArray MacroTransition::first_directs() {return first_directs_;}
DirectsArray MacroTransition::repeat_directs() {return repeat_directs_;}
CapturesArray MacroTransition::first_captures() {return first_captures_;}
CapturesArray MacroTransition::repeat_captures() {return repeat_captures_;}
EmptiesArray MacroTransition::empties() {return empties_;}

MacroState* MacroTransition::next_state() {return next_;}

} // end namespace rematch