#ifndef AUTOMATA__MACRODFA__MACROSTATE
#define AUTOMATA__MACRODFA__MACROSTATE

#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>

#include "automata/macrodfa/macrotransition.hpp"

class DetState;

class MacroState {
 public:
  MacroState(DetState *state);
  MacroState(std::vector<DetState*> states);

  MacroTransition* next_transition(char a);

  void add_transition(char a);

  std::vector<DetState*>& states();

 private:
  int id_;
  std::unordered_map<unsigned char, MacroTransition> transitions_;
  std::vector<DetState*> states_;
}; // end class MacroState

#endif // AUTOMATA__MACRODFA__MACROSTATE