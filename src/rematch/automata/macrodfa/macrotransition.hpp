#ifndef AUTOMATA_MACRODFA_MACROTRANSITION_HPP
#define AUTOMATA_MACRODFA_MACROTRANSITION_HPP

#include <bitset>
#include <vector>
#include <memory>

#include "automata/dfa/dstate.hpp"
#include "structs/prevector.hpp"

namespace rematch {

class MacroState;

struct MTAbs {
  MTAbs() = default;
  MTAbs(DState& from, DState& to) : from(&from), to(&to) {}

  DState* from;
  DState* to;
};

struct MTCapture: public MTAbs {
  MTCapture() = default;
  MTCapture(DState& from, std::bitset<32> S, DState& to)
    : MTAbs(from, to), S(S) {}

  std::bitset<32> S;
};

struct MTDirect: public MTAbs {
  MTDirect() = default;
  MTDirect(DState& from, DState& to)
    : MTAbs(from, to) {}
};

enum MacroType {
  kOneDirect,
  kTwoDirects,
  kOther
};

#ifdef MACRO_TRANSITIONS_RAW_ARRAYS
using DirectsArray = MTDirect*;
using CapturesArray = MTCapture*;
using EmptiesArray = DState**;
#else
using DirectsArray = rematch::prevector<MTDirect>;
using CapturesArray = rematch::prevector<MTCapture>;
#endif

class MacroTransition {
 public:
  MacroTransition() = default;

  MacroTransition(size_t nfirstdirects, size_t nrepeatdirects,
                  size_t nfirstcaptures, size_t nrepeatcaptures,
                  size_t nempties);

  void add_direct(DState& from, DState& to, bool first);
  void add_capture(DState& from, std::bitset<32> S, DState& to, bool first);
  void add_empty(DState& from);

  DirectsArray first_directs();
  DirectsArray repeat_directs();
  CapturesArray first_captures();
  CapturesArray repeat_captures();
  EmptiesArray empties();

  DirectsArray directs() { return nullptr; }
  CapturesArray captures() { return nullptr; }

  void set_next_state(MacroState* ms);

  MacroState* next_state();

  int nfirstdirects_ = 0;
  int nfirstcaptures_ = 0;
  int nrepeatdirects_ = 0;
  int nrepeatcaptures_ = 0;
  int nempties_ = 0;

  size_t ndirects_ = 0;
  size_t ncaptures_ = 0;

 private:
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  MTDirect *first_directs_;
  MTDirect *repeat_directs_;
  MTCapture *first_captures_;
  MTCapture *repeat_captures_;
  DState **empties_;
  #else
  rematch::prevector<MTDirect> directs_;
  rematch::prevector<MTCapture> captures_;
  #endif

  MacroState* next_;

  // MacroType type_;
}; // end class MacroTransition

} // end namespace rematch

#endif // AUTOMATA_MACRODFA_MACROTRANSITION_HPP
