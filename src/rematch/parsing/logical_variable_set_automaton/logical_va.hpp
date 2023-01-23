#ifndef AUTOMATA_NFA_LogicalVA_HPP
#define AUTOMATA_NFA_LogicalVA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>

#include "parsing/logical_variable_set_automaton/logical_va_state.hpp"

namespace rematch {

class LogicalVA {
  /* A basic implementation of a Variable Automaton  */
  friend class ExtendedVA;

 private:

  // Creates a new LogicalVAState for the automaton.
  LogicalVAState* new_state();

  LogicalVAState* init_state_;
  LogicalVAState* accepting_state_;

  bool has_epsilon_ = false;

public:
  LogicalVAState* initial_state() const { return init_state_; }
  LogicalVAState* accepting_state() const { return accepting_state_; }

 public:
  std::vector<LogicalVAState*> states;

  // Empty LogicalVA construction (only one LogicalVAState)
  LogicalVA();

  // Copy constructor
  LogicalVA(const LogicalVA &A);

  // Atomic VA
  LogicalVA(unsigned int code);


  // Transforms the automaton graph to a trimmed automaton. This being that every
  // state is reacheable from the initial state, and the final state is reachable
  // from every state.
  void trim();

  /****************************************************************************/
  /* LogicalVA operations, all modify the current LogicalVA to get the result */
  /* (the operations are inplace)                                             */
  /****************************************************************************/

  // Inplace transformation from R to RR'
  void cat(LogicalVA &a2);
  // Equivalent to R1|R2
  void alter(LogicalVA &a2);
  // Equivalent to R*
  void kleene();
  // Equivalent to R+
  void strict_kleene();
  // Equivalent to R?
  void optional();
  // Equivalent to !x{R}
  void assign(std::bitset<64> open_code, std::bitset<64> close_code);
  // Inplace transformation from R to R{min,max}
  void repeat(int min, int max);

  // Remove capture transitions as if they were instantaneous (epsilon labeled)
  void remove_captures();

  // Removes epsilon transitions.
  void remove_epsilon();

  void relabel_states();

  bool has_epsilon() const { return has_epsilon_; }

  friend std::ostream& operator<<(std::ostream& os, LogicalVA const &A);

};

}
#endif // AUTOMATA_NFA_LogicalVA_HPP

