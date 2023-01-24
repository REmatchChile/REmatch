#include "segment_identification/search_variable_set_automaton/dfa/search_dfa.hpp"

#include <string>
#include <vector>
#include <set>

namespace rematch {

SearchDFA::SearchDFA(SearchVA const &A)
    : has_epsilon_(A.has_epsilon()),
      sVA_(A) {
  initial_state_ = new_dstate();
	initial_state_->set_initial(true);
}

SearchDFAState* SearchDFA::new_dstate() {
  auto np = new SearchDFAState();
  states.push_back(np);
  return np;
}

SearchDFAState* SearchDFA::next_state(SearchDFAState *state, char a) {
/**
 * Assumptions:
 *   the ids are from 0 upwards (necessary to use the subsetBitset.)
 */

  if (state->transitions[a] != nullptr) {
    return state->transitions[a];
  }

  std::set<SearchNFAState*> newSubset;  // Store the next subset
	std::vector<bool> subsetBitset(sVA_.size());  // Subset bitset representation

  visit_states(state->subset(), newSubset, subsetBitset, a);
  visit_states(initial_state_->subset(), newSubset, subsetBitset, a);

  auto new_state = new SearchDFAState(newSubset);
  state->transitions[a] = new_state;
  return new_state;
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
