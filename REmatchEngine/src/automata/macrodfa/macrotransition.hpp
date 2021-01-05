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

class MacroTransition {
 public:
  MacroTransition();

  void add_direct(DetState& from, DetState& to);
  void add_capture(DetState& from, std::bitset<32> S, DetState& to);

  MTCapture* captures();
  MTDirect* directs();

  void set_next_state(MacroState* ms);

  MacroState* next_state();

  size_t directs_idx_;
  size_t captures_idx_;

 private:
  MTDirect directs_[10];
  MTCapture captures_[10];

  MacroState* next_;

  // MacroType type_;
}; // end class MacroTransition

#endif // AUTOMATA_MACRODFA_MACROTRANSITION