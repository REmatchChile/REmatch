#ifndef DETAUTOMATON_HPP
#define DETAUTOMATON_HPP

#include <string>
#include <vector>
#include <map>

#include "automata/eva.hpp"

class VariableFactory;
class DetState;

class DetAutomaton {
  public:
    DetState* initState;
    VariableFactory* vFact;

    // Vector of pointers of States for resizing:
    std::vector<DetState*> states;
    std::vector<DetState*> finalStates;

    std::vector<std::string> varNames;

    // Empty Automaton construction (only one state)
    DetAutomaton(VariableFactory* vf);
    DetAutomaton(ExtendedVA &a);

    std::string pprint();

    void computeOneReached();

    size_t size() const {return states.size();}

};

#endif
