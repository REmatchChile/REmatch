#ifndef AUTOMATA_NFA_LVA_HPP
#define AUTOMATA_NFA_LVA_HPP

#include <bitset>
#include <memory>
#include <string>
#include <vector>

namespace rematch {

class VariableFactory;
class FilterFactory;
class ExtendedVA;

class LogicalVA {
  /* A basic implementation of a Variable Automaton  */
  friend class ExtendedVA;

 public:
  class State;
  struct Capture;
  struct Filter;
  struct Epsilon;

  std::vector<State*> states;

  // Empty LogicalVA construction (only one State)
  LogicalVA();

  // Copy constructor
  LogicalVA(const LogicalVA &A);

  // Atomic VA
  LogicalVA(unsigned int code);

  // Sets a reference for the associated factories.
  void set_factories(std::shared_ptr<VariableFactory> v,
                     std::shared_ptr<FilterFactory> f);

  // Transforms the automaton graph to a trimmed automaton. This being that
  // every state is reacheable from the initial state, and the final state is
  // reachable from every state.
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
  void assign(std::bitset<32> open_code, std::bitset<32> close_code);
  // Inplace transformation from R to R{min,max}
  void repeat(int min, int max);

  // Remove capture transitions as if they were instantaneous (epsilon labeled)
  void remove_captures();

  // Removes epsilon transitions.
  void remove_epsilon();

  void relabel_states();

  std::shared_ptr<VariableFactory> varFactory() const { return vfactory_; }
  std::shared_ptr<FilterFactory> filterFactory() const { return ffactory_; }

  State *initial_state() const { return init_state_; }
  State *accepting_state() const { return accepting_state_; }

  bool has_epsilon() const { return has_epsilon_; }

  friend std::ostream &operator<<(std::ostream &os, LogicalVA const &A);

private:
  // Creates a new State for the automaton.
  State *new_state();

  State *init_state_;
  State *accepting_state_;

  bool has_epsilon_ = false;

  std::shared_ptr<VariableFactory> vfactory_;
  std::shared_ptr<FilterFactory> ffactory_;
};

} // end namespace rematch

#endif // AUTOMATA_NFA_LVA_HPP
