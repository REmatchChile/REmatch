#ifndef LVA_HPP
#define LVA_HPP

#include <string>
#include <vector>
#include <memory>

namespace rematch {

class VariableFactory;
class FilterFactory;
class LVAState;

class LogicalVA {
  /* A basic implementation of a Variable Automaton  */
 public:
  std::vector<LVAState*> states;
  std::vector<LVAState*> finalStates;

  // Empty LogicalVA construction (only one LVAState)
  LogicalVA();

  LogicalVA(VariableFactory &vFact, FilterFactory &fFact);

  LogicalVA(std::string pattern, bool raw=false);

  // Atomic LogicalVA construction
  LogicalVA(const char &a);
  LogicalVA(const char &a, VariableFactory &vFact, FilterFactory &fFact);
  LogicalVA(int spec, bool negated, VariableFactory &vFact, FilterFactory &fFact);

  // Copy constructor
  // LogicalVA(const LogicalVA &A);

  // Computes epsilon transitions between capture states.
  void adapt_capture_jumping();

  // LogicalVA operations, all modify the current LogicalVA to get the result
  // (the operations are inplace)

  void cat(LogicalVA &a2);                 // Equivalent to R1.R2 in VarRegEx
  void alter(LogicalVA &a2);               // Equivalent to R1|R2 in VarRegEx
  void kleene();                           // Equivalent to R* in VarRegEx
  void strict_kleene();                    // Equivalent to R+ in VarRegEx
  void optional();                         // Equivalent to R? in VarRegEx
  void assign(std::string varName);  // Equivalent to !x{R} in VarRegEx

  std::string pprint();

  std::shared_ptr<VariableFactory> varFactory() {return v_factory_;}
  std::shared_ptr<FilterFactory> filterFactory() {return f_factory_;}

  LVAState* initState() const {return init_state_;}

 private:

  LVAState* init_state_;

  std::shared_ptr<VariableFactory> v_factory_;
  std::shared_ptr<FilterFactory> f_factory_;
};

} // end namespace rematch

#endif