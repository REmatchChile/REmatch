#include "detmanager.hpp"

#include <string>
#include <functional>
#include <stack>
#include <random>
#include <vector>

#include "parse/regex/parser.hpp"
#include "automata/nfa/eva.hpp"
#include "automata/nfa/state.hpp"
#include "automata/dfa/dfa.hpp"
#include "automata/dfa/detstate.hpp"
#include "det/setstate.hpp"
#include "captures.hpp"

namespace rematch {

DetManager::DetManager(std::string pattern, bool raw_automata) {
	auto lva = regex2LVA(pattern);

	if (raw_automata) lva->adapt_capture_jumping();

	nfa_ = std::make_unique<ExtendedVA>(*lva);
	dfa_ = std::make_unique<DFA>(*nfa_);
	mdfa_ = std::make_unique<MacroDFA>();

	variable_factory_ = nfa_->varFactory();
	filter_factory_ = nfa_->filterFactory();

	// Init determinization
	std::set<State*> new_subset;
	new_subset.insert(nfa_->initState());

	SetState* ss = new SetState(*nfa_, new_subset);
	DetState *q = dfa_->initState();
	q->setSubset(ss);

	dstates_table_[ss->bitstring] = q;

	if(q->isFinal) {
		dfa_->finalStates.push_back(q);
	}
	// computeCaptures(q, q, 0);
	// q->add_direct(0, q);
}


void DetManager :: computeCaptures(DetState* p, DetState* q, char a) {
	/* Computes the reachable subsets from the deterministic state q through
	captures, stores them if necessary and connects q to the computed
	deterministic states thourgh deterministic capture transitions */

	std::unordered_map<std::bitset<32>, std::set<State*>> captureList;
	std::unordered_map<std::bitset<32>, std::set<State*>>::iterator it;

	for(auto &extState: q->ss->subset) {
		for(auto &capture: extState->captures) {

			it = captureList.find(capture->code);

			if(it == captureList.end()) {
				it = captureList.emplace(capture->code, std::set<State*>()).first;
			}
			it->second.insert(capture->next);
		}
	}

	for(std::pair<std::bitset<32>, std::set<State*>> el: captureList){
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

	std::set<State*> newSubset;  // Store the next subset
	BitsetWrapper subsetBitset(nfa_->size());  // Subset bitset representation

	for(auto &state: q->ss->subset) {
		for(auto &filter: state->filters) {
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
		// for(auto &state: nq->ss->subset){
			// if(!state->filters.empty() || state->isFinal){
				q->add_direct(a, nq);
				// break;
			// }
		// }
	} else {
		q->add_empty(a, nq);
	}

	return q->next_transition(a);
}

MacroTransition* DetManager::next_macro_transition(MacroState *p, char a) {

	// Set to store the key
	std::set<size_t> dstates_key;

	// Set to store the reached states
	std::set<DetState*> dstates_storage;

	std::vector<std::pair<DetState*, DetState*>> first_storage, repeats_storage;

	int nfirstdirects = 0, nrepeatdirects = 0,
	    nfirstcaptures = 0, nrepeatcaptures = 0, nempties = 0;

	for(auto &state: p->states()) {
		// Classic on-the-fly determinization
		auto nextTransition = state->next_transition(a);

		if(nextTransition == nullptr) {
			nextTransition = this->next_transition(state, a);
		}

		if(nextTransition->type_ & Transition::kDirect) {
			auto res = dstates_key.insert(nextTransition->direct_->id);
			if (!res.second)
				nrepeatdirects++;
			else
				nfirstdirects++;
		} else if (nextTransition->type_ == Transition::kEmpty) {
			nempties++;
		} if (nextTransition->type_ & Transition::kSingleCapture) {
			auto res = dstates_key.insert(nextTransition->capture_->next->id);
			if (!res.second)
				nrepeatcaptures++;
			else
				nfirstcaptures++;
		} else if(nextTransition->type_ & Transition::kMultiCapture) {
			for(auto &capture: *nextTransition->captures_) {
				auto res = dstates_key.insert(capture->next->id);
				if (!res.second)
					nrepeatcaptures++;
				else
					nfirstcaptures++;
			}
		}
	}

	// Alloc a new MacroTransition
	std::shared_ptr<MacroTransition> mtrans = std::make_shared<MacroTransition>(nfirstdirects, nrepeatdirects,
																																						  nfirstcaptures, nrepeatcaptures, nempties);

	for(auto &state: p->states()) {
		// Classic on-the-fly determinization
		auto nextTransition = state->next_transition(a);

		if(nextTransition->type_ & Transition::kDirect) {
			auto res = dstates_storage.insert(nextTransition->direct_);
			if (res.second)
				mtrans->add_direct(*state, *nextTransition->direct_, true);
			else
				mtrans->add_direct(*state, *nextTransition->direct_, false);
		} else if(nextTransition->type_ == Transition::kEmpty) {
			mtrans->add_empty(*state);
		} if (nextTransition->type_ & Transition::kSingleCapture) {
			auto res = dstates_storage.insert(nextTransition->capture_->next);
			if (res.second)
				mtrans->add_capture(*state, nextTransition->capture_->S, *nextTransition->capture_->next, true);
			else
				mtrans->add_capture(*state, nextTransition->capture_->S, *nextTransition->capture_->next, false);
		} else if(nextTransition->type_ & Transition::kMultiCapture) {
			for(auto &capture: *nextTransition->captures_) {
				auto res = dstates_storage.insert(capture->next);
				if (res.second)
					mtrans->add_capture(*state, capture->S, *capture->next, true);
				else
					mtrans->add_capture(*state, capture->S, *capture->next, true);
			}
		}
	}

	// Pass up to a vector
	std::vector<size_t> real_dstates_key(dstates_key.begin(), dstates_key.end());

	// Sorting needed to compute the correct key
	std::sort(real_dstates_key.begin(), real_dstates_key.end());

	auto found = mstates_table_.find(real_dstates_key);

	MacroState *q;

	// Check if not inside table already
	if(found == mstates_table_.end()) {
		// Convert set to vector
		std::vector<DetState*> real_dstates_storage(dstates_storage.begin(), dstates_storage.end());

		// Create the new MacroState
		q = mdfa_->add_state(real_dstates_storage);

		// Insert new MacroState in table
		mstates_table_.insert(std::pair<std::vector<size_t>, MacroState*>(real_dstates_key, q));
	} else {
		q = found->second;
	}

	mtrans->set_next_state(q);

	// mtrans->set_type();

	p->add_transition(a, mtrans);

	return mtrans.get();
}

MacroState* DetManager::compute_drop_super_finals(MacroState *ms) {
	std::set<size_t> dstates_key;
	std::set<DetState*> dstates_storage;
	for(auto &dstate: ms->states()) {
		auto ns = dstate->drop_super_finals();
		if(ns == nullptr) {
			ns = this->compute_drop_super_finals(dstate);
		}
		if(!ns->empty()) {
			dstates_storage.insert(ns);
			dstates_key.insert(ns->id);
		}
	}

	// Pass up to a vector
	std::vector<size_t> real_dstates_key(dstates_key.begin(), dstates_key.end());

	// Sorting needed to compute the correct key
	std::sort(real_dstates_key.begin(), real_dstates_key.end());

	auto found = mstates_table_.find(real_dstates_key);

	MacroState *q;

	// Check if not inside table already
	if(found == mstates_table_.end()) {
		// Convert set to vector
		std::vector<DetState*> real_dstates_storage(dstates_storage.begin(), dstates_storage.end());

		// Create the new MacroState
		q = mdfa_->add_state(real_dstates_storage);

		// Insert new MacroState in table
		mstates_table_.insert(std::pair<std::vector<size_t>, MacroState*>(real_dstates_key, q));
	} else {
		q = found->second;
	}

	ms->set_drop_super_finals(q);

	return q;
}

DetState* DetManager::compute_drop_super_finals(DetState *q) {

	std::set<State*> newSubset;  // Store the next subset
	BitsetWrapper subsetBitset(nfa_->size());  // Subset bitset representation

	for(auto &state: q->ss->subset) {
		if (!state->isSuperFinal){
			newSubset.insert(state);
			subsetBitset.set(state->id, true);
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

	q->set_drop_super_finals(nq);

	return nq;
}

} // end namespace rematch