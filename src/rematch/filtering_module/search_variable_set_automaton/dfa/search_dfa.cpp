#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include "search_dfa.hpp"

namespace REmatch {
inline namespace filtering_module {

SearchDFA::SearchDFA(LogicalVA const &logical_va, DFAStateLimitChecker dfa_states_checker)
    : sVA_(SearchNFA(logical_va)), dfa_states_checker_(dfa_states_checker) {
  initial_state = create_initial_dfa_state();
  current_state = initial_state;
  initial_nfa_states.push_back(sVA_.initial_state());
}

SearchDFA::~SearchDFA() {
  for (auto& state : states) {
    delete state;
  }
}

SearchDFAState* SearchDFA::create_initial_dfa_state() {
  auto np = new SearchDFAState();
  states.push_back(np);
  dfa_states_checker_.count_state();
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

  //std::cout << "Finding next state of " << (int) ((uint8_t) a) << std::endl;
  if (current_state->transitions[(uint8_t) a] != nullptr) {
    //std::cout << "Next state already computed for " << (int) ((uint8_t) a) << std::endl;
    current_state = current_state -> transitions[(uint8_t) a];
    return current_state;
  }

  std::set<SearchNFAState*> newSubset;  // Store the next subset
	std::vector<bool> subsetBitset(sVA_.size() + 1);  // Subset bitset representation

  visit_states(current_state->subset(), newSubset, subsetBitset, a);
  //std::cout << "newSubset size: " << newSubset.size() << " for: " << (int) ((uint8_t) a) << std::endl;
  if (newSubset.empty()) {
    //std::cout << "Saying it is ends " << (int) ((uint8_t) a) << std::endl;
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
    dfa_states_checker_.count_state();
  }

  current_state->transitions[(uint8_t) a] = new_state;
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

size_t SearchDFA::get_search_nfa_size() {
  return sVA_.size();
}

}
}
