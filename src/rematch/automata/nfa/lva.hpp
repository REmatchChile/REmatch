#ifndef SRC_REMATCH_AUTOMATA_NFA_LVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_LVA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>

namespace rematch {

class VariableFactory;
class FilterFactory;
class State;
class ExtendedVA;

// FIXME: LogicalVA with two special states, a final state WITHOUT transitions
// leaving from it. And unanchored evaluation must be implicit in the
// automaton (i.e. we don't have to have loops in the initial/final state).

class LogicalVA {
  /* A basic implementation of a Variable Automaton  */
  friend class ExtendedVA;

 public:
  std::vector<State*> states;

  // Empty LogicalVA construction (only one State)
  LogicalVA();

  // Copy constructor
  LogicalVA(const LogicalVA &A);

  // Atomic VA
  LogicalVA(uint code);

  // Sets a reference for the associated factories.
  void set_factories(std::shared_ptr<VariableFactory> v,
                     std::shared_ptr<FilterFactory> f);

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
  // Equivalent to R1|R2 in VarRegEx
  void alter(LogicalVA &a2);
  // Equivalent to R* in VarRegEx
  void kleene();
  // Equivalent to R+ in VarRegEx
  void strict_kleene();
  // Equivalent to R?
  void optional();
  // Equivalent to !x{R}
  void assign(std::bitset<32> open_code, std::bitset<32> close_code);
  // Inplace transformation from R to R{min,max}
  void repeat(int min, int max);

  // Remove capture transitions as if they were instantaneous (epsilon labeled)
  void remove_captures();

  std::string pprint();

  std::shared_ptr<VariableFactory> varFactory() const {return vfactory_;}
  std::shared_ptr<FilterFactory> filterFactory() const {return ffactory_;}

  State* initial_state() const {return init_state_;}
  State* accepting_state() const { return final_state_; }

  bool has_epsilon() const { return has_epsilon_; }

 private:

  // Creates a new State for the automaton.
  State* new_state();

  // Creates new final state for the automaton.
  State* new_final_state();

  State* init_state_;
  State* final_state_;

  bool has_epsilon_ = false;

  std::shared_ptr<VariableFactory> vfactory_;
  std::shared_ptr<FilterFactory> ffactory_;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_LVA_HPP
