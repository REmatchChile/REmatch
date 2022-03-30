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

namespace rematch {

class VariableFactory;
class DetState;
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

  void computeOneReached();

  size_t size() const { return states.size(); }

  bool only_capture_init_state() const;

  // ---  Determinization  ---  //

  Transition* next_transition(DState* q, char a);
  void computeCaptures(DState* p, DState* q, char a);

 private:
  // Utility to print a transition
  void print_transition(std::ostream& os, DetState* from, char a,
                        DetState* to, std::bitset<32> S);


  // The starting state of the dfa
  DState* init_state_;

  ExtendedVA const &eVA_;

  DFAStatesTable dstates_table_;

  DFACaptureStatesTable init_eval_states_;

  // Access to variable and filter factory
  std::shared_ptr<VariableFactory> variable_factory_;
  std::shared_ptr<FilterFactory> ffactory_;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_DFA_DFA_HPP
