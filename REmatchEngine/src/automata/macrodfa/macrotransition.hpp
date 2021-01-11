#ifndef AUTOMATA_MACRODFA_MACROTRANSITION
#define AUTOMATA_MACRODFA_MACROTRANSITION

#include <bitset>
#include <vector>
#include <memory>

#include "automata/dfa/detstate.hpp"
#include "structs/prevector.hpp"

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
#else
using DirectsArray = rematch::prevector<MTDirect>;
using CapturesArray = rematch::prevector<MTCapture>;
#endif

class MacroTransition {
 public:
  MacroTransition();

  MacroTransition(size_t ndirects, size_t ncaptures);

  void add_direct(DetState& from, DetState& to);
  void add_capture(DetState& from, std::bitset<32> S, DetState& to);

  DirectsArray directs();
  CapturesArray captures();

  void set_next_state(MacroState* ms);

  MacroState* next_state();

  size_t ndirects_;
  size_t ncaptures_;

 private:
  #ifdef MACRO_TRANSITIONS_RAW_ARRAYS
  MTDirect *directs_;
  MTCapture *captures_;
  #else
  rematch::vector<MTDirect> directs_;
  rematch::vector<MTCapture> captures_;
  #endif

  MacroState* next_;

  // MacroType type_;
}; // end class MacroTransition

#endif // AUTOMATA_MACRODFA_MACROTRANSITION