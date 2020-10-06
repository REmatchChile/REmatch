#ifndef AUTOMATA_MACRODFA_MACROTRANSITION
#define AUTOMATA_MACRODFA_MACROTRANSITION

#include <bitset>
#include <vector>
#include <memory>

#include "automata/detstate.hpp"

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
  MacroTransition(char a);

  void add_direct(char a, DetState& from, DetState& to);
  void add_capture(char a, DetState& from, std::bitset<32> S, DetState& to);

  std::vector<MTDirect>& directs();
  std::vector<MTCapture>& captures();

  MacroState* next_state();

 private:
  unsigned char label_;
  std::vector<MTDirect> directs_;
  std::vector<MTCapture> captures_;

  std::shared_ptr<MacroState> next_;
}; // end class MacroTransition

#endif // AUTOMATA_MACRODFA_MACROTRANSITION