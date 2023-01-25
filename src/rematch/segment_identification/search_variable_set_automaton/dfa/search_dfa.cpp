#include "segment_identification/search_variable_set_automaton/dfa/search_dfa.hpp"

#include <string>
#include <vector>
#include <set>
#include <iostream>

namespace rematch {

SearchDFA::SearchDFA(LogicalVA const &logical_va)
    : sVA_(SearchNFA(logical_va)) {
  initial_state = create_initial_dfa_state();
	initial_state->set_initial();
  current_state = initial_state;
}

SearchDFAState* SearchDFA::create_initial_dfa_state() {
  auto np = new SearchDFAState(sVA_.initial_state());
  states.push_back(np);
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
	std::vector<bool> subsetBitset(sVA_.size());  // Subset bitset representation

  visit_states(current_state->subset(), newSubset, subsetBitset, a);
  visit_states(initial_state->subset(), newSubset, subsetBitset, a);

  std::cout << "new subsetBitset[1]: " << subsetBitset[1] << std::endl;

  auto new_state = new SearchDFAState(newSubset);
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
