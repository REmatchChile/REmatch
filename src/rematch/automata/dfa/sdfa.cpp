#include "sdfa.hpp"

#include <string>
#include <vector>
#include <set>

#include "factories/factories.hpp"

namespace rematch {

SearchDFA::SearchDFA(SearchVA const &A)
    : has_epsilon_(A.has_epsilon()),
      sVA_(A),
      vfactory_(A.variable_factory()),
      ffactory_(A.filter_factory()) {
  initial_state_ = new_dstate();
  initial_state_->add_state(A.initial_state());

	initial_state_->set_initial(true);

  dstates_table_[initial_state_->bitmap()] = initial_state_;

}

SDState* SearchDFA::new_dstate() {
  SDState* np = new SDState(sVA_.size());
  states.push_back(np);
  return np;
}

SDState* SearchDFA::next_state(SDState *q, char a) {

	std::vector<bool> triggered_filters = ffactory_->applyFilters(a);

	std::set<State*> new_subset;  // Store the next subset
	std::vector<bool> new_bitset(sVA_.size());  // Subset bitset representation

	for(auto &state: q->subset()) {
		for(auto &filter: state->filters) {
			if(triggered_filters[filter->code] && !new_bitset[filter->next->id]) {
				new_subset.insert(filter->next);
				new_bitset[filter->next->id] = true;
			}
		}
	}

	auto found = dstates_table_.find(new_bitset);

	SDState* nq;

	if(found == dstates_table_.end()) { // Check if already stored subset
		nq = new SDState(sVA_.size(), new_subset);

		dstates_table_[nq->bitmap()] = nq;

		states.push_back(nq);

		if(nq->accepting())
			final_states.push_back(nq);

	} else {
		nq = found->second;
	}

	q->set_transition(a, nq);

	return nq;
}

} // end namespace rematch
