#include "macrotransition.hpp"

namespace rematch {

MacroTransition::MacroTransition(size_t nfirstdirects, size_t nrepeatdirects,
                                 size_t nfirstcaptures, size_t nrepeatcaptures,
                                 size_t nempties) {
  first_directs_.reserve(nfirstdirects);
  first_captures_.reserve(nfirstcaptures);
  repeat_directs_.reserve(nrepeatdirects);
  repeat_captures_.reserve(nrepeatcaptures);
}

void MacroTransition::add_direct(DState& from, DState& to, bool first) {
  if (first)
    first_directs_.emplace_back(from, to);
  else
    repeat_directs_.emplace_back(from, to);
}

void MacroTransition::add_capture(DState& from, std::bitset<32> S,
                                  DState& to, bool first) {
  if (first)
    first_captures_.emplace_back(from, S, to);
  else
    repeat_captures_.emplace_back(from, S, to);
}

void MacroTransition::add_empty(DState& from) {
  empties_.emplace_back(&from);
}

void MacroTransition::set_next_state(MacroState* ms) {next_ = ms;}

DirectsArray MacroTransition::first_directs() {return first_directs_;}
DirectsArray MacroTransition::repeat_directs() {return repeat_directs_;}
CapturesArray MacroTransition::first_captures() {return first_captures_;}
CapturesArray MacroTransition::repeat_captures() {return repeat_captures_;}
EmptiesArray MacroTransition::empties() {return empties_;}

MacroState* MacroTransition::next_state() {return next_;}

} // end namespace rematch