#ifndef AUTOMATA_DFA_SDFA_HPP
#define AUTOMATA_DFA_SDFA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_transition.hpp"
#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_state.hpp"

#include "segment_identification/search_variable_set_automaton/nfa/search_nfa.hpp"

namespace rematch {

using DFAStatesTableBools = std::unordered_map<std::vector<bool>, SearchDFAState*>;

class SearchDFA {

 public:
  // Vector of pointers of States for resizing:
  std::vector<SearchDFAState*> states;
  std::vector<SearchDFAState*> final_states;

  std::vector<std::string> varNames;
 private:
  // The starting state of the dfa
  SearchDFAState* initial_state_;

  bool has_epsilon_;

  SearchVA const &sVA_;

  DFAStatesTableBools dstates_table_;


 public:

  SearchDFA(SearchVA const &A);

  //  ---  Getters  ---  //

  SearchDFAState* initial_state() {return initial_state_;}

  // @brief Check if the empty word is inside the automaton's language
  bool has_epsilon() const { return has_epsilon_; }

  std::string pprint();

  // @brief Number of states in the automaton's graph
  size_t size() const {return states.size();}

  // ---  Determinization  ---  //

  SearchDFAState* next_state(SearchDFAState* q, char a);

 private:

  SearchDFAState* new_dstate();
};

} // end namespace rematch

#endif // AUTOMATA_DFA_SDFA_HPP
