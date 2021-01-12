#ifndef AUTOMATA__DFA__DFA_HPP
#define AUTOMATA__DFA__DFA_HPP

#include <string>
#include <vector>
#include <memory>

#include "automata/eva.hpp"

class VariableFactory;
class DetState;

class DFA {
 public:

  // Vector of pointers of States for resizing:
  std::vector<DetState*> states;
  std::vector<DetState*> finalStates;

  std::vector<std::string> varNames;

  // Empty Automaton construction (only one state)
  DFA(VariableFactory* vf);
  DFA(ExtendedVA &a);

  // Getter for init state
  DetState* initState() {return init_state_;};

  std::string pprint();

  void computeOneReached();

  size_t size() const {return states.size();}

 private:
  // Utility to print a transition
  void print_transition(std::ostream& os, DetState* from, char a, DetState* to, std::bitset<32> S);

  // The starting state of the dfa
  DetState* init_state_;

  // Access to variable factory
  std::shared_ptr<VariableFactory> variable_factory_;

};

#endif
