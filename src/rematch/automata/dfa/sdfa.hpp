#ifndef AUTOMATA_DFA_SDFA_HPP
#define AUTOMATA_DFA_SDFA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "automata/dfa/transition.hpp"
#include "automata/dfa/dstate.hpp"

#include "automata/nfa/sva.hpp"
#include "regex/regex_options.hpp"

namespace rematch {

class VariableFactory;
class DetState;
class ExtendedVA;

using DFAStatesTableBools = std::unordered_map<std::vector<bool>, DState*>;

class SearchDFA {
 public:
  // Vector of pointers of States for resizing:
  std::vector<DState*> states;
  std::vector<DState*> final_states;

  std::vector<std::string> varNames;

  SearchDFA(SearchVA const &A, Anchor a = Anchor::kAnchorBoth);

  //  ---  Getters  ---  //

  DState* initial_state() {return initial_state_;}

  // @brief Check if the empty word is inside the automaton's language
  bool has_epsilon() const { return has_epsilon_; }

  std::string pprint();

  // @brief Number of states in the automaton's graph
  size_t size() const {return states.size();}

  // ---  Determinization  ---  //

  DState* next_state(DState* q, char a);

 private:

  DState* new_dstate();

  Anchor anchor_;

  // The starting state of the dfa
  DState* initial_state_;

  bool has_epsilon_;

  SearchVA const &sVA_;

  DFAStatesTableBools dstates_table_;

  // Access to variable and filter factory
  std::shared_ptr<VariableFactory>  vfactory_;
  std::shared_ptr<FilterFactory>    ffactory_;
};

} // end namespace rematch

#endif // AUTOMATA_DFA_SDFA_HPP
