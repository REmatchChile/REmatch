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

using DirectsArray = std::vector<MTDirect>;
using CapturesArray = std::vector<MTCapture>;
using EmptiesArray = std::vector<DState*>;

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

  void set_next_state(MacroState* ms);

  MacroState* next_state();

 private:
  DirectsArray  first_directs_;
  DirectsArray  repeat_directs_;
  CapturesArray first_captures_;
  CapturesArray repeat_captures_;
  EmptiesArray  empties_;

  MacroState* next_;

  // MacroType type_;
}; // end class MacroTransition

} // end namespace rematch

#endif // AUTOMATA_MACRODFA_MACROTRANSITION_HPP
