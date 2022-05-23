#ifndef SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
#define SRC_REMATCH_AUTOMATA_DFA_DFA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "automata/dfa/transition.hpp"
#include "regex/regex_options.hpp"
#include "factories/factories.hpp"
#include "automata/dfa/dstate.hpp"

namespace rematch {

class VariableFactory;
class ExtendedVA;

using DFAStatesTable = std::unordered_map<std::vector<bool>, DState*>;
using DFACaptureStatesTable = std::vector<std::pair<DState*, std::bitset<32>>>;

class DFA {
 public:

  // Vector of pointers of States for resizing:
  std::vector<DState*> states;
  std::vector<DState*> finalStates;

  std::vector<std::string> varNames;

  DFA(ExtendedVA const &A);

  // Getter for init state
  DState* init_state() {return init_state_;};

  DFACaptureStatesTable init_eval_states() { return init_eval_states_; }

  std::string pprint();

  size_t size() const { return states.size(); }

  // ---  Determinization  ---  //


  // @brief Compute an on-the-fly determinization
  //
  // @param q State from which to compute the next state
  // @param a Read character to follow the transitions
  // @return Transition* The correct deterministic transition from q reading a
  Transition* next_transition(DState* q, char a);

 private:
  // Utility to print a transition
  void print_transition(std::ostream& os, DState* from, char a,
                        DState* to, std::bitset<32> S);

  // Computes the reachable subsets from the deterministic state q
	// through captures, stores them if necessary and connects p to the computed
	// deterministic states thourgh deterministic capture transitions.
  // * Used inernally
  void compute_captures(DState* p, DState* q, char a);

  // The starting state of the dfa
  DState* init_state_;

  ExtendedVA const &eVA_;

  std::map<std::vector<bool>, DState*> dstates_table_;

  DFACaptureStatesTable init_eval_states_;

  // Access to variable and filter factory
  std::shared_ptr<VariableFactory> variable_factory_;
  std::shared_ptr<FilterFactory> ffactory_;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
