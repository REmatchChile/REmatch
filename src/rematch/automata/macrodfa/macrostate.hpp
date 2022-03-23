#ifndef AUTOMATA__MACRODFA__MACROSTATE
#define AUTOMATA__MACRODFA__MACROSTATE

#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>

#include "automata/macrodfa/macrotransition.hpp"

namespace rematch {

class DetState;

class MacroState {

  using TransitionsMap = std::array<std::shared_ptr<MacroTransition>, 128>;

 public:
  MacroState(DetState *state);
  MacroState(std::vector<DetState*> states);

  MacroTransition* next_transition(char a);

  void add_transition(char a, std::shared_ptr<MacroTransition> tr);

  std::vector<DetState*>& states();

  MacroState* drop_super_finals() const {return drop_super_finals_;}
  void set_drop_super_finals(MacroState* ms) { drop_super_finals_ = ms; }

  friend std::ostream& operator<<(std::ostream &os, MacroState const &m) {
    os << "{";
    for(auto &dstate: m.states_) {
      if(dstate == *m.states_.begin())
        os << dstate->label;
      else
        os << " " << dstate->label;
    }
    os << "}";
    return os;
  }

 private:
  int id_;
  TransitionsMap transitions_;
  std::vector<DetState*> states_;
  MacroState* drop_super_finals_ = nullptr;
}; // end class MacroState

} // end namespace rematch

#endif // AUTOMATA__MACRODFA__MACROSTATE