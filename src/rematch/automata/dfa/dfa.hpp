#ifndef SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
#define SRC_REMATCH_AUTOMATA_DFA_DFA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "automata/dfa/transition.hpp"

namespace rematch {

class VariableFactory;
class DetState;
class ExtendedVA;

using DFAStatesTable = std::unordered_map<BitsetWrapper, DetState*>;

class DFA {
 public:

  // Vector of pointers of States for resizing:
  std::vector<DetState*> states;
  std::vector<DetState*> finalStates;

  std::vector<std::string> varNames;

  DFA(ExtendedVA const &A);

  // Getter for init state
  DetState* initState() {return init_state_;};

  std::string pprint();

  void computeOneReached();

  size_t size() const {return states.size();}

  // ---  Determinization  ---  //

  Transition* next_transition(DetState* q, char a);
  void computeCaptures(DetState* p, DetState* q, char a);

  DetState* compute_drop_super_finals(DetState *q);

 private:
  // Utility to print a transition
  void print_transition(std::ostream& os, DetState* from, char a, DetState* to, std::bitset<32> S);

  // The starting state of the dfa
  DetState* init_state_;

  ExtendedVA const &eVA_;

  DFAStatesTable dstates_table_;

  // Access to variable and filter factory
  std::shared_ptr<VariableFactory> variable_factory_;
  std::shared_ptr<FilterFactory> ffactory_;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
