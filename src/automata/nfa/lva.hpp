#ifndef LVA_HPP
#define LVA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>

namespace rematch {

class VariableFactory;
class FilterFactory;
class State;

class LogicalVA {
  /* A basic implementation of a Variable Automaton  */
 public:
  std::vector<State*> states;
  std::vector<State*> finalStates;

  // Empty LogicalVA construction (only one State)
  LogicalVA();

  LogicalVA(const LogicalVA &A);

  // LogicalVA(std::string pattern, bool raw=false);

  // Atomic VA
  LogicalVA(uint code);

  // Sets a reference for the associated factories.
  void set_factories(std::shared_ptr<VariableFactory> v,
                     std::shared_ptr<FilterFactory> f);

  // Copy constructor
  // LogicalVA(const LogicalVA &A);

  // Computes epsilon transitions between capture states.
  void adapt_capture_jumping();

  // LogicalVA operations, all modify the current LogicalVA to get the result
  // (the operations are inplace)

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

  std::string pprint();

  std::shared_ptr<VariableFactory> varFactory() {return v_factory_;}
  std::shared_ptr<FilterFactory> filterFactory() {return f_factory_;}

  State* initState() const {return init_state_;}

 private:

  // Creates a new State for the automaton.
  State* new_state();

  // Creates new final state for the automaton.
  State* new_final_state();

  State* init_state_;

  std::shared_ptr<VariableFactory> v_factory_;
  std::shared_ptr<FilterFactory> f_factory_;
};

} // end namespace rematch

#endif