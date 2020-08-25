#include "detmanager.hpp"

#include <string>
#include <functional>
#include <stack>
#include <random>

#include "parser/parser.hpp"
#include "automata/eva.hpp"
#include "automata/detautomaton.hpp"
#include "automata/detstate.hpp"
#include "det/setstate.hpp"

// #include <boost/multiprecision/cpp_int.hpp>
// #include <boost/multiprecision/cpp_bin_float.hpp>

// using namespace boost::multiprecision;

DetManager::DetManager(std::string pattern, bool raw_automata) {
	LogicalVA lva = regex2LVA(pattern);

	if (raw_automata) lva.adapt_capture_jumping();

	nfa_ = std::make_unique<ExtendedVA>(lva);
	dfa_ = std::make_unique<DetAutomaton>(*nfa_);

	variable_factory_ = nfa_->varFactory();
	filter_factory_ = nfa_->filterFactory();

	// Init determinization
	std::set<LVAState*> new_subset;
	new_subset.insert(nfa_->initState());

	SetState* ss = new SetState(*nfa_, new_subset);
	DetState *q = dfa_->initState();
	q->setSubset(ss);

	dstates_table_[ss->bitstring] = q;

	if(q->isFinal) {
		dfa_->finalStates.push_back(q);
	}
	computeCaptures(q, q, 0);
	q->add_direct(0, q);
}


void DetManager :: computeCaptures(DetState* p, DetState* q, char a) {
	/* Computes the reachable subsets from the deterministic state q through
	captures, stores them if necessary and connects q to the computed
	deterministic states thourgh deterministic capture transitions */

	std::unordered_map<std::bitset<32>, std::set<LVAState*>> captureList;
	std::unordered_map<std::bitset<32>, std::set<LVAState*>>::iterator it;

	for(auto &extState: q->ss->subset) {
		for(auto &capture: extState->c) {

			it = captureList.find(capture->code);

			if(it == captureList.end()) {
				it = captureList.emplace(capture->code, std::set<LVAState*>()).first;
			}
			it->second.insert(capture->next);
		}
	}

	for(std::pair<std::bitset<32>, std::set<LVAState*>> el: captureList){
		SetState* nss = new SetState(*nfa_, el.second);

		/* Check if subset is not on hash table */

		auto found = dstates_table_.find(nss->bitstring);

		if(found == dstates_table_.end()) {
			DetState* nq = new DetState(nss);
			dstates_table_[nss->bitstring] = nq;

			dfa_->states.push_back(nq);

			if (nq->isFinal) {
				dfa_->finalStates.push_back(nq);
			}
		}

		DetState* r = dstates_table_[nss->bitstring]; // This is the deterministic state where the capture reaches

		p->add_capture(a, el.first, r);
	}
}


rematch::Transition* DetManager::next_transition(DetState *q, char a) {

	BitsetWrapper charBitset = filter_factory_->applyFilters(a);

	std::set<LVAState*> newSubset;  // Store the next subset
	BitsetWrapper subsetBitset(nfa_->size());  // Subset bitset representation

	for(auto &state: q->ss->subset) {
		for(auto &filter: state->f) {
			if(charBitset.get(filter->code) && !subsetBitset.get(filter->next->id)) {
				newSubset.insert(filter->next);
				subsetBitset.set(filter->next->id, true);
			}
		}
	}

	auto found = dstates_table_.find(subsetBitset);

	DetState* nq;

	if(found == dstates_table_.end()) { // Check if already stored subset
		SetState* ss = new SetState(*nfa_, newSubset);
		nq = new DetState(ss);

		dstates_table_[ss->bitstring] = nq;

		dfa_->states.push_back(nq);

		if(nq->isFinal) {
			dfa_->finalStates.push_back(nq);
		}
	} else {
		nq = found->second;
	}
	if(nq->ss->isNonEmpty) {
		computeCaptures(q, nq, a);
		for(auto &state: nq->ss->subset){
			if(!state->f.empty()){
				q->add_direct(a, nq);
				break;
			}
		}
	} else {
		q->add_empty(a, nq);
	}

	return q->next_transition(a);
}
