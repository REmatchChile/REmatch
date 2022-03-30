#include "dfa.hpp"

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <list>
#include <cassert>
#include <map>

#include "structs/dag/nodelist.hpp"
#include "factories/factories.hpp"
#include "automata/nfa/eva.hpp"
#include "automata/dfa/transition.hpp"
#include "automata/dfa/dstate.hpp"
#include "automata/nfa/state.hpp"

namespace rematch {

DFA::DFA(ExtendedVA const &A)
    : init_state_(nullptr),
      eVA_(A),
      variable_factory_(A.varFactory()),
      ffactory_(A.filterFactory()) {

  // Code initial state
  std::set<State*> new_subset;
	new_subset.insert(eVA_.init_state());

	init_state_ = new DState(eVA_.size(), new_subset);

	dstates_table_[init_state_->bitmap()] = init_state_;

	// Compute init eval states

	std::unordered_map<std::bitset<32>, std::set<State*>> reach_captures;

	for(auto &capture: eVA_.init_state()->captures) {
		auto res = reach_captures.emplace(capture->code, std::set<State*>());
		res.first->second.insert(capture->next);
	}

	for(auto &elem: reach_captures) {
		DState* nq = new DState(eVA_.size(), elem.second);

		auto found = dstates_table_.find(nq->bitmap());

		if(found == dstates_table_.end()) {
			dstates_table_[nq->bitmap()] = nq;

			states.push_back(nq);

			if (nq->accepting())
				finalStates.push_back(nq);
		} else {
			delete nq;
			nq = found->second;
		}

		init_eval_states_.emplace_back(std::make_pair(nq, elem.first));
	}
}

Transition* DFA::next_transition(DState *q, char a) {

	std::vector<bool> charBitset = ffactory_->applyFilters(a);

	std::set<State*> newSubset;  // Store the next subset
	std::vector<bool> subsetBitset(eVA_.size(), false);  // Subset bitset representation

	for(auto &state: q->subset()) {

		for(auto &filter: state->filters) {
			if(charBitset[filter->code] && !subsetBitset[filter->next->id]) {
				newSubset.insert(filter->next);
				subsetBitset[filter->next->id] = true;
			}
		}
	}

	auto found = dstates_table_.find(subsetBitset);

	DState* nq;

	if(found == dstates_table_.end()) { // Check if already stored subset
		nq = new DState(eVA_.size(), newSubset);

		dstates_table_[nq->bitmap()] = nq;

		states.push_back(nq);

		if(nq->accepting()) {
			finalStates.push_back(nq);
		}
	} else {
		nq = found->second;
	}
	if(!nq->empty_subset()) {
		computeCaptures(q, nq, a);
		// for(auto &state: nq->ss->subset){
			// if(!state->filters.empty() || state->isFinal){
				q->add_direct(a, nq);
				// break;
			// }
		// }
	} else {
		q->add_empty(a);
	}

	return q->next_transition(a);
}

void DFA::computeCaptures(DState* p, DState* q, char a) {
	/* Computes the reachable subsets from the deterministic state q through
	captures, stores them if necessary and connects q to the computed
	deterministic states thourgh deterministic capture transitions */

	std::unordered_map<std::bitset<32>, std::set<State*>> captureList;
	std::unordered_map<std::bitset<32>, std::set<State*>>::iterator it;

	for(auto &extState: q->subset()) {
		for(auto &capture: extState->captures) {

			it = captureList.find(capture->code);

			if(it == captureList.end()) {
				it = captureList.emplace(capture->code, std::set<State*>()).first;
			}
			it->second.insert(capture->next);
		}
	}

	for(std::pair<std::bitset<32>, std::set<State*>> el: captureList){
		DState* nq = new DState(eVA_.size(), el.second);

		/* Check if subset is not on hash table */

		auto found = dstates_table_.find(nq->bitmap());

		if(found == dstates_table_.end()) {
			dstates_table_[nq->bitmap()] = nq;

			states.push_back(nq);

			if (nq->accepting()) {
				finalStates.push_back(nq);
			}
		}

		DState* r = dstates_table_[nq->bitmap()]; // This is the deterministic state where the capture reaches

		p->add_capture(a, el.first, r);
	}
}

bool DFA::only_capture_init_state() const {
	return eVA_.init_state()->filters.empty();
}

} // end namespace rematch
