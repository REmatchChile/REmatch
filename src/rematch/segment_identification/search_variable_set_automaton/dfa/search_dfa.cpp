#include "segment_identification/search_variable_set_automaton/dfa/search_dfa.hpp"

#include <string>
#include <vector>
#include <set>

namespace rematch {

SearchDFA::SearchDFA(SearchVA const &A)
    : has_epsilon_(A.has_epsilon()),
      sVA_(A) {
  initial_state_ = new_dstate();
  initial_state_->add_state(A.initial_state());

	initial_state_->set_initial(true);

  dstates_table_[initial_state_->bitmap()] = initial_state_;

}

SearchDFAState* SearchDFA::new_dstate() {
  auto np = new SearchDFAState(sVA_.size());
  states.push_back(np);
  return np;
}

SearchDFAState* SearchDFA::next_state(SearchDFAState *q, char a) {

  // TODO, use logic:
  // This is next_delta from the paper
	//std::vector<bool> triggered_filters = ffactory_->applyFilters(a);

	//std::set<State*> newSubset;  // Store the next subset
	//std::vector<bool> subsetBitset(sVA_.size());  // Subset bitset representation

	//for(auto &state: q->subset()) {

		//for(auto &filter: state->filters) {
			//if(triggered_filters[filter->code] && !subsetBitset[filter->next->id]) {
				//newSubset.insert(filter->next);
				//subsetBitset[filter->next->id] = true;
			//}
		//}
	//}

	//auto found = dstates_table_.find(subsetBitset);

  //std::vector<State*> new_subset_vect(newSubset.begin(), newSubset.end());

	//SearchDFAState* nq;

	//if(found == dstates_table_.end()) { // Check if already stored subset
		//nq = new SearchDFAState(sVA_.size(), new_subset_vect);

		//dstates_table_[nq->bitmap()] = nq;

		//states.push_back(nq);

		//if(nq->accepting()) {
			//final_states.push_back(nq);
		//}
	//} else {
		//nq = found->second;
	//}

	//q->add_direct(a, nq);

	//return nq;
}

} // end namespace rematch
