#ifndef SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
#define SRC_REMATCH_AUTOMATA_DFA_DFA_HPP

#include <bitset>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

#include "automata/dfa/transition.hpp"
#include "factories/factories.hpp"
#include "regex/regex_options.hpp"

namespace rematch {

class VariableFactory;
class ExtendedVA;

class DFA {

 public:
  class State;

  using DFAStatesTable = std::unordered_map<std::vector<bool>, State*>;
  using DFACaptureStatesTable = std::vector<std::pair<State*, std::bitset<32>>>;

  // Vector of pointers of States for resizing:
  std::vector<State*> states;
  std::vector<State*> finalStates;

  std::vector<std::string> varNames;

  DFA(ExtendedVA const &A);

  // Getter for init state
  State *init_state() { return init_state_; };

  DFACaptureStatesTable init_eval_states() { return init_eval_states_; }

  std::string pprint();

  size_t size() const { return states.size(); }

  // ---  Determinization  ---  //

  // @brief Compute an on-the-fly determinization
  //
  // @param q State from which to compute the next state
  // @param a Read character to follow the transitions
  // @return Transition* The correct deterministic transition from q reading a
  Transition next_transition(abstract::DState *q, char a);

private:
  // Utility to print a transition
  void print_transition(std::ostream &os, State *from, char a, State *to,
                        std::bitset<32> S);

  // Computes the reachable subsets from the deterministic state q
  // through captures, stores them if necessary and connects p to the computed
  // deterministic states thourgh deterministic capture transitions.
  // * Used inernally
  void compute_captures(State *p, State *q, char a);

  // The starting state of the dfa
  State *init_state_;

  ExtendedVA const &eVA_;

  std::map<std::vector<bool>, State*> dstates_table_;

  DFACaptureStatesTable init_eval_states_;

  // Access to variable and filter factory
  std::shared_ptr<VariableFactory> variable_factory_;
  std::shared_ptr<FilterFactory> ffactory_;

}; // end class DFA

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
