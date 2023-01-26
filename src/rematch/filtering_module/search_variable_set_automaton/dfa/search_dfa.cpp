#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"

#include <string>
#include <vector>
#include <set>
#include <iostream>

namespace rematch {

SearchDFA::SearchDFA(LogicalVA const &logical_va)
    : sVA_(SearchNFA(logical_va)) {
  initial_state = create_initial_dfa_state();
  current_state = initial_state;
  initial_nfa_states.push_back(sVA_.initial_state());
}

SearchDFAState* SearchDFA::create_initial_dfa_state() {
  auto np = new SearchDFAState();
  states.push_back(np);
	np->set_initial();
  // To store that the state ends the sVA_size() position is used.
  std::vector<bool> initial_state_bitset(sVA_.size() + 1);
  initial_state_bitset[sVA_.initial_state()->id] = true;
  initial_state_bitset[sVA_.size()] = true; // it starts as ends.
  dfa_state_catalog[initial_state_bitset] = initial_state;
  return np;
}

SearchDFAState* SearchDFA::next_state(char a) {
/**
 * Assumptions:
 *   the ids are from 0 upwards (necessary to use the subsetBitset.)
 */

  if (current_state->transitions[a] != nullptr) {
    current_state = current_state -> transitions[a];
    return current_state;
  }

  std::set<SearchNFAState*> newSubset;  // Store the next subset
	std::vector<bool> subsetBitset(sVA_.size() + 1);  // Subset bitset representation

  visit_states(current_state->subset(), newSubset, subsetBitset, a);
  if (newSubset.empty()) {
    subsetBitset[sVA_.size()] = true;
  }
  visit_states(initial_nfa_states, newSubset, subsetBitset, a);

  // find the new_state if it was already created.
  SearchDFAState *new_state;
  if (dfa_state_catalog.count(subsetBitset)) {
    new_state = dfa_state_catalog.at(subsetBitset);
  }
  else {
    new_state = new SearchDFAState(
        newSubset, subsetBitset[sVA_.size()]);
    dfa_state_catalog[subsetBitset] = new_state;
    states.push_back(new_state);
  }

  current_state->transitions[a] = new_state;
  current_state = new_state;
  return current_state;
}

void SearchDFA::visit_states(
    std::vector<SearchNFAState*> const &states,
    std::set<SearchNFAState*> &subset,  // Store the next subset
    std::vector<bool> &subsetBitset,
    char &a){  // Subset bitset representation
	for(auto &state: states) {
		for(auto &filter: state->filters) {
      if (!subsetBitset[filter->next->id] && filter->charclass.contains(a)) {
        subset.insert(filter->next);
        subsetBitset[filter->next->id] = true;
      }
    }
  }
}



} // end namespace rematch
