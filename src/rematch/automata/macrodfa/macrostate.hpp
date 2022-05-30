#ifndef AUTOMATA__MACRODFA__MACROSTATE
#define AUTOMATA__MACRODFA__MACROSTATE

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "automata/macrodfa/macrotransition.hpp"

namespace rematch {

class DState;

class MacroState {

  using TransitionsMap = std::array<std::shared_ptr<MacroTransition>, 128>;

public:
  MacroState(DState *state);
  MacroState(std::vector<DState *> states);

  MacroTransition *next_transition(char a);

  void add_transition(char a, std::shared_ptr<MacroTransition> tr);

  std::vector<DState *> &states();

  friend std::ostream &operator<<(std::ostream &os, MacroState const &m) {
    os << "{";
    for (auto &dstate : m.states_) {
      if (dstate == *m.states_.begin())
        os << dstate->label();
      else
        os << " " << dstate->label();
    }
    os << "}";
    return os;
  }

  static int current_ID;

private:
  int id_ = current_ID++;
  TransitionsMap transitions_;
  std::vector<DState *> states_;
}; // end class MacroState

} // end namespace rematch

#endif // AUTOMATA__MACRODFA__MACROSTATE