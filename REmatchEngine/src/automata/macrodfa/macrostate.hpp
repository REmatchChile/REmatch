#ifndef AUTOMATA__MACRODFA__MACROSTATE
#define AUTOMATA__MACRODFA__MACROSTATE

#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>

#include "automata/macrodfa/macrotransition.hpp"

class DetState;

class MacroState {

  using TransitionsMap = std::unordered_map<unsigned char, std::shared_ptr<MacroTransition>>;

 public:
  MacroState(DetState *state);
  MacroState(std::vector<DetState*> states);

  MacroTransition* next_transition(char a);

  void add_transition(char a, std::shared_ptr<MacroTransition> tr);

  std::vector<DetState*>& states();

 private:
  int id_;
  TransitionsMap transitions_;
  std::vector<DetState*> states_;
}; // end class MacroState

#endif // AUTOMATA__MACRODFA__MACROSTATE