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

  // Vector of pointers of States for resizing:
  std::vector<DetState*> states;
  std::vector<DetState*> finalStates;

  std::vector<std::string> varNames;

  // Empty Automaton construction (only one state)
  DetAutomaton(VariableFactory* vf);
  DetAutomaton(ExtendedVA &a);

  // Getter for init state
  DetState* initState() {return init_state_;};

  std::string pprint();

  void computeOneReached();

  size_t size() const {return states.size();}

 private:
  DetState* init_state_;
  std::shared_ptr<VariableFactory> variable_factory_;

};

#endif
