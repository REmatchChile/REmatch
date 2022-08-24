#ifndef AUTOMATA__MACRODFA__MACROSTATE
#define AUTOMATA__MACRODFA__MACROSTATE

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "automata/macrodfa/macrotransition.hpp"

#include "automata/dfa/dfa.hpp"
#include "automata/dfa/dstate.hpp"

namespace rematch {

class MacroState {

  using TransitionsMap = std::array<std::shared_ptr<MacroTransition>, 128>;

public:
  MacroState(DFA::State *state);
  MacroState(std::vector<DFA::State *> states);

  MacroTransition *next_transition(char a);

  void add_transition(char a, std::shared_ptr<MacroTransition> tr);

  std::vector<DFA::State *> &states();

  friend std::ostream &operator<<(std::ostream &os, MacroState const &m) {
    os << "{";
    for (auto &dstate : m.states_) {
      if (dstate == *m.states_.begin())
        os << " ";
      else
        os << " ";
    }
    os << "}";
    return os;
  }

  static int current_ID;

private:
  int id_ = current_ID++;
  TransitionsMap transitions_;
  std::vector<DFA::State *> states_;
}; // end class MacroState

} // end namespace rematch

#endif // AUTOMATA__MACRODFA__MACROSTATE