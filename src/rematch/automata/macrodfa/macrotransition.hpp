#ifndef AUTOMATA_MACRODFA_MACROTRANSITION_HPP
#define AUTOMATA_MACRODFA_MACROTRANSITION_HPP

#include <bitset>
#include <memory>
#include <vector>

#include "automata/dfa/dstate.hpp"

namespace rematch {

class MacroState;

struct MTAbs {
  MTAbs() = default;
  MTAbs(DFA::State &from, DFA::State &to) : from(&from), to(&to) {}

  DFA::State *from;
  DFA::State *to;
};

struct MTCapture : public MTAbs {
  MTCapture() = default;
  MTCapture(DFA::State &from, std::bitset<32> S, DFA::State &to)
      : MTAbs(from, to), S(S) {}

  std::bitset<32> S;
};

struct MTDirect : public MTAbs {
  MTDirect() = default;
  MTDirect(DFA::State &from, DFA::State &to) : MTAbs(from, to) {}
};

enum MacroType { kOneDirect, kTwoDirects, kOther };

using DirectsArray = MTDirect *;
using CapturesArray = MTCapture *;
using EmptiesArray = DFA::State **;

class MacroTransition {
public:
  MacroTransition() = default;

  MacroTransition(size_t nfirstdirects, size_t nrepeatdirects,
                  size_t nfirstcaptures, size_t nrepeatcaptures,
                  size_t nempties);

  void add_direct(DFA::State &from, DFA::State &to, bool first);
  void add_capture(DFA::State &from, std::bitset<32> S, DFA::State &to, bool first);
  void add_empty(DFA::State &from);

  DirectsArray first_directs();
  DirectsArray repeat_directs();
  CapturesArray first_captures();
  CapturesArray repeat_captures();
  EmptiesArray empties();

  DirectsArray directs() { return nullptr; }
  CapturesArray captures() { return nullptr; }

  void set_next_state(MacroState *ms);

  MacroState *next_state();

  int nfirstdirects_ = 0;
  int nfirstcaptures_ = 0;
  int nrepeatdirects_ = 0;
  int nrepeatcaptures_ = 0;
  int nempties_ = 0;

  size_t ndirects_ = 0;
  size_t ncaptures_ = 0;

private:
  MTDirect *first_directs_;
  MTDirect *repeat_directs_;
  MTCapture *first_captures_;
  MTCapture *repeat_captures_;
  DFA::State **empties_;

  MacroState *next_;

  // MacroType type_;
}; // end class MacroTransition

} // end namespace rematch

#endif // AUTOMATA_MACRODFA_MACROTRANSITION_HPP
