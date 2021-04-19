#ifndef AUTOMATA_MACRODFA_MACROTRANSITION
#define AUTOMATA_MACRODFA_MACROTRANSITION

#include <bitset>
#include <vector>
#include <memory>

#include "automata/dfa/detstate.hpp"
#include "structs/prevector.hpp"

namespace rematch {

class MacroState;

struct MTCapture {
  MTCapture() = default;
  MTCapture(DetState& from, std::bitset<32> S, DetState& to)
    : from(&from), to(&to), S(S) {}

  DetState* from;
  DetState* to;
  std::bitset<32> S;
};

struct MTDirect {
  MTDirect() = default;
  MTDirect(DetState& from, DetState& to)
    : from(&from), to(&to) {}

  DetState* from;
  DetState* to;
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
  MacroTransition();

  MacroTransition(size_t ndirects, size_t ncaptures, size_t nempties);

  void add_direct(DetState& from, DetState& to);
  void add_capture(DetState& from, std::bitset<32> S, DetState& to);
  void add_empty(DetState& from);

  DirectsArray directs();
  CapturesArray captures();
  EmptiesArray empties();

  void set_next_state(MacroState* ms);

  MacroState* next_state();

  size_t ndirects_;
  size_t ncaptures_;
  size_t nempties_;

 private:
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  MTDirect *directs_;
  MTCapture *captures_;
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