#ifndef AUTOMATA_DFA_SDFA_HPP
#define AUTOMATA_DFA_SDFA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_state.hpp"

#include "segment_identification/search_variable_set_automaton/nfa/search_nfa.hpp"

namespace rematch {

using DFAStatesTableBools = std::unordered_map<std::vector<bool>, SearchDFAState*>;

class SearchDFA {

 public:
  // Vector of pointers of States for resizing:
  std::vector<SearchDFAState*> states;
  std::vector<SearchDFAState*> final_states;

 private:
  // The starting state of the dfa
  SearchDFAState* current_state;
  SearchDFAState* initial_state;
  SearchNFA sVA_;


 public:

  SearchDFA(LogicalVA const &logical_va);

  //  ---  Getters  ---  //

  SearchDFAState* get_initial_state() {return initial_state;}

  // @brief Check if the empty word is inside the automaton's language

  std::string pprint();

  // @brief Number of states in the automaton's graph
  size_t size() const {return states.size();}

  // ---  Determinization  ---  //

  SearchDFAState* next_state(char a);
  void reset() { current_state = initial_state;}

 private:

  SearchDFAState* new_dstate();
  void visit_states(
    std::vector<SearchNFAState*> const &states_subset,
    std::set<SearchNFAState*> &subset,  // Store the next subset
    std::vector<bool> &subsetBitset,
    char &a);  // Subset bitset representation
};

} // end namespace rematch

#endif // AUTOMATA_DFA_SDFA_HPP
