#ifndef AUTOMATA_MACRODFA_MACRODFA_HPP
#define AUTOMATA_MACRODFA_MACRODFA_HPP

#include <vector>
#include <functional>
#include <memory>

#include "automata/macrodfa/macrostate.hpp"
#include "automata/dfa/dfa.hpp"                     // For DFA reference
#include "util/util.hpp"                            // For VectorHasher

namespace rematch {

class MacroSegmentEvaluator;

class MacroDFA {
 public:
  using MacroStatesTable = std::unordered_map<std::vector<size_t>, MacroState*,
																							                    util::VectorHasher>;
  friend MacroSegmentEvaluator;
  MacroDFA(DFA& dA);

  // @brief Add a state to the set of states of the macro-automaton. Returning
  // the new macro-state.
  //
  // @param state a deterministic state.
  // @return MacroState* The new macro-state.
  MacroState* add_state(DState *state);
  MacroState* add_state(std::vector<DState*> states);

  void set_as_init(MacroState *ms);

  MacroTransition* next_transition(MacroState *ms, char a);

  MacroState& get_init_state();

  size_t size() {return states_.size();}

  std::vector<MacroState*>& states() { return states_; }

 private:
  MacroState *init_state_; // Raw ptr
  std::vector<MacroState*> states_;

  DFA& dfa_;                              // DFA reference

  MacroStatesTable mstates_table_;        // Determinization index table

  // std::vector<std::shared_ptr<MacroState>> final_states_;
}; // end class MacroDFA

} // end namespace rematch

#endif // AUTOMATA_MACRODFA_MACRODFA_HPP
