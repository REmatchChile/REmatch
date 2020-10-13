#ifndef AUTOMATA_MACRODFA_MACROTRANSITION
#define AUTOMATA_MACRODFA_MACROTRANSITION

#include <bitset>
#include <vector>
#include <memory>

#include "automata/dfa/detstate.hpp"

class MacroState;

struct MTCapture {
  MTCapture(DetState& from, std::bitset<32> S, DetState& to)
    : from(from), to(to), S(S) {}

  DetState& from;
  DetState& to;
  std::bitset<32> S;
};

struct MTDirect {
  MTDirect(DetState& from, DetState& to)
    : from(from), to(to) {}

  DetState& from;
  DetState& to;
};

class MacroTransition {
 public:
  MacroTransition();

  void add_direct(DetState& from, DetState& to);
  void add_capture(DetState& from, std::bitset<32> S, DetState& to);

  std::vector<MTDirect>& directs();
  std::vector<MTCapture>& captures();

  void set_next_state(MacroState* ms);

  MacroState* next_state();

 private:
  std::vector<MTDirect> directs_;
  std::vector<MTCapture> captures_;

  MacroState* next_;
}; // end class MacroTransition

#endif // AUTOMATA_MACRODFA_MACROTRANSITION