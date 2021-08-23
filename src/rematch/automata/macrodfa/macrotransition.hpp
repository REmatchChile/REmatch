#ifndef AUTOMATA_MACRODFA_MACROTRANSITION
#define AUTOMATA_MACRODFA_MACROTRANSITION

#include <bitset>
#include <vector>
#include <memory>

#include "automata/dfa/detstate.hpp"
#include "structs/prevector.hpp"

namespace rematch {

class MacroState;

struct MTAbs {
  MTAbs() = default;
  MTAbs(DetState& from, DetState& to) : from(&from), to(&to) {}

  DetState* from;
  DetState* to;
};

struct MTCapture: public MTAbs {
  MTCapture() = default;
  MTCapture(DetState& from, std::bitset<32> S, DetState& to)
    : MTAbs(from, to), S(S) {}

  std::bitset<32> S;
};

struct MTDirect: public MTAbs {
  MTDirect() = default;
  MTDirect(DetState& from, DetState& to)
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
using EmptiesArray = DetState**;
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

  void add_direct(DetState& from, DetState& to, bool first);
  void add_capture(DetState& from, std::bitset<32> S, DetState& to, bool first);
  void add_empty(DetState& from);

  DirectsArray first_directs();
  DirectsArray repeat_directs();
  CapturesArray first_captures();
  CapturesArray repeat_captures();
  EmptiesArray empties();

  DirectsArray directs() { return nullptr; }
  CapturesArray captures() { return nullptr; }

  void set_next_state(MacroState* ms);

  MacroState* next_state();

  size_t nfirstdirects_ = 0;
  size_t nfirstcaptures_ = 0;
  size_t nrepeatdirects_ = 0;
  size_t nrepeatcaptures_ = 0;
  size_t nempties_ = 0;

  size_t ndirects_ = 0;
  size_t ncaptures_ = 0;

 private:
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  MTDirect *first_directs_;
  MTDirect *repeat_directs_;
  MTCapture *first_captures_;
  MTCapture *repeat_captures_;
  DetState **empties_;
  #else
  rematch::prevector<MTDirect> directs_;
  rematch::prevector<MTCapture> captures_;
  #endif

  MacroState* next_;

  // MacroType type_;
}; // end class MacroTransition

} // end namespace rematch

#endif // AUTOMATA_MACRODFA_MACROTRANSITION