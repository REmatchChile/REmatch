#ifndef LVA_HPP
#define LVA_HPP


#include <string>
#include <vector>

class VariableFactory;
class FilterFactory;
class LVAState;

class LogicalVA {
  /* A basic implementation of a Variable Automaton  */
  public:
    int numStates, numFilters, numCaptures, numEpsilons;

    VariableFactory *vFact;
    FilterFactory *fFact;

    LVAState* initState;
    std::vector<LVAState*> states;
    std::vector<LVAState*> finalStates;

    // Empty LogicalVA construction (only one LVAState)
    LogicalVA();

    LogicalVA(VariableFactory &vFact, FilterFactory &fFact);

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
};

#endif