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
	computeCaptures(q);
}

void DetManager :: computeFullDetAutomaton() {
	std::stack<DetState*> stateList;
	stateList.push(dfa_->initState());
	DetState* currState;
	// size_t numFilters = variable_factory_->size();
	size_t numStates = nfa_->size();

	// Get all possible transitions
	BitsetWrapper newSubsetBitset(numStates);

	while(!stateList.empty()) {
		currState = stateList.top(); stateList.pop();
		// std::cout << "Checking state: " << currState->id << '\n';
		for(auto &capture: currState->c) {
			if(!capture->next->mark) {
				capture->next->mark = true;
				stateList.push(capture->next);
			}
		}

		std::unordered_map<size_t, BitsetWrapper> nextSubsetMap;

		for(auto &state: currState->ss->subset) {
			for(auto &filter: state->f) {
				auto it = nextSubsetMap.find(filter->code);
				if(it == nextSubsetMap.end())
					it = nextSubsetMap.emplace(filter->code,
											   BitsetWrapper(numStates)).first;
				it->second.set(filter->next->id, true);
			}
		}

		for(auto &chrbset: all_chars_table_) {
			newSubsetBitset.clear();
			newSubsetBitset.resize(numStates);
			for(auto &keyValue : nextSubsetMap) {
				if(chrbset.first.get(keyValue.first))
					newSubsetBitset |= keyValue.second;
			}

			// std::cout << "Charbset: " << chrbset << "\n";

			// Look for DetState in hashtable
			auto found = dstates_table_.find(newSubsetBitset);

			// If not found add to hashtable
			if (found == dstates_table_.end()) {
				SetState *ss = new SetState(*nfa_, nfa_->getSubset(newSubsetBitset));
				DetState *nq = new DetState(ss);

				found = dstates_table_.insert({ss->bitstring, nq}).first;

				dfa_->states.push_back(nq);

				if(nq->isFinal)
					dfa_->finalStates.push_back(nq);

				computeCaptures(nq);

				nq->mark = true;
				stateList.push(nq);
			}

			// Add the transition
			currState->addFilter(chrbset.first, found->second);
		}
	}

  // std::cout << "Determinization: " << t.elapsed() << '\n';
}

char DetManager :: chooseFromCharBitset(BitsetWrapper bs) {
  std::vector<char> &intersection = all_chars_table_[bs];
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, intersection.size()-1);

	return intersection[dis(gen)];

}

std::string DetManager :: uniformSample(size_t n) {
  // Timer t;
	// std::unordered_map<DetState*, std::pair<std::vector<BitsetWrapper>, cpp_int>> T, Tprim;
	// T.insert({dfa_->initState(), std::make_pair(std::vector<BitsetWrapper>(), 1)});
	// DetState *q, *p;
	// const BitsetWrapper *a;
	// std::vector<BitsetWrapper> *w, *u;
	// cpp_int N, M, S;

	// // FIXME: Change logic to only cpp_int usage
  // double prob;

	// double c;

	// std::random_device rd;
	// std::mt19937 gen(rd());
	// std::uniform_real_distribution<> dis(0.0, 1.0);

	// dfa_->computeOneReached();

  // // std::cout << "Init algo: " << t.elapsed() << '\n';
  // // t.reset();

	// for(size_t i=1; i <= n; i++) {
	// 	for(auto &state_pair: T) {
	// 		p = state_pair.first; w = &state_pair.second.first; N = state_pair.second.second;
	// 		for(auto &bset_state: p->oneReached) {
	// 			a = &bset_state.first; q = bset_state.second;
  //       S = all_chars_table_[*a].size();
	// 			if(!Tprim.count(q)) {
	// 				std::vector<BitsetWrapper> wa = *w;
	// 				wa.push_back(*a);
	// 				Tprim[q] = std::make_pair(wa, N*S);
	// 			}
	// 			else {
	// 				u = &Tprim[q].first; M = Tprim[q].second;
	// 				c = dis(gen); prob = double(M)/double(M+N*S);
	// 				if ( c <= prob) {
	// 					std::vector<BitsetWrapper> ua = *u;
	// 					Tprim[q] = std::make_pair(ua, M+N*S);
	// 				}
	// 				else {
	// 					std::vector<BitsetWrapper> wa = *w;
	// 					wa.push_back(*a);
	// 					Tprim[q] = std::make_pair(wa, M+N*S);
	// 				}
	// 			}
	// 		}
	// 	}

	// 	T = Tprim;
	// 	Tprim.clear();
	// }

  // // std::cout << "Populate hash tables: " << t.elapsed() << '\n';
  // // t.reset();

	// M = 0; u = nullptr;
	// for(auto &state_pair: T) {
	// 	p = state_pair.first; w = &state_pair.second.first; N = state_pair.second.second;
	// 	if(p->isFinal) {
	// 		c = dis(gen); prob = double(N)/double(M+N);
	// 		if (c <= prob)
	// 			u = w;
	// 		M += N;
	// 	}
	// }

  // std::cout << "Get bitset string: " << t.elapsed() << '\n';
  // t.reset();

	std::stringstream ss;
	// char d;
	// if(u != nullptr) {
	// 	for(auto &bs: *u) {
	// 		ss << chooseFromCharBitset(bs);
	// 	}
	// }

  // std::cout << "Get full string: " << t.elapsed() << '\n';
  // t.reset();

	return ss.str();
}

DetState* DetManager :: getNextSubset(SetState* ss, BitsetWrapper charBitset) {
	/* Gets next subset from a subset ss if filter bitset b is read */

	std::set<LVAState*> newSubset;  // Store the next subset
	BitsetWrapper subsetBitset(nfa_->size());  // Subset bitset representation

	for(auto &state: ss->subset) {
		for(auto &filter: state->f) {
			if(charBitset.get(filter->code) && !subsetBitset.get(filter->next->id)) {
				// std::cout << "\t\tFilter code " << filter->code << " triggered at state " << state->id << std::endl;
				newSubset.insert(filter->next);
				subsetBitset.set(filter->next->id, true);
			}
		}
	}

	auto found = dstates_table_.find(subsetBitset);

	if(found == dstates_table_.end()) { // Check if already stored subset
		SetState* ss = new SetState(*nfa_, newSubset);
		DetState* nq = new DetState(ss);

		dstates_table_[ss->bitstring] = nq;

		dfa_->states.push_back(nq);

		if(nq->isFinal) {
			dfa_->finalStates.push_back(nq);
		}

		computeCaptures(nq);
	}

	return dstates_table_[subsetBitset];
}


void DetManager :: computeCaptures(DetState* q) {
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
		if(!dstates_table_.count(nss->bitstring)) {
			DetState* nq = new DetState(nss);
			dstates_table_[nss->bitstring] = nq;

			dfa_->states.push_back(nq);

			if (nq->isFinal) {
				dfa_->finalStates.push_back(nq);
			}

      computeCaptures(nq);

		}

		// TODO: Delete nss if subset was already on hash table

		DetState* p = dstates_table_[nss->bitstring]; // This is the deterministic state where the capture reaches

		q->addCapture(el.first, p);
	}
}

DetState* DetManager :: getNextDetState(DetState* s, BitsetWrapper charBitset) {
	/* Gets next deterministic state from s if reading a*/

	// std::cout << "Computing determinization for " << charBitset << " at detstate " << *s << std::endl;

	DetState* q = getNextSubset(s->ss, charBitset);

	// std::cout << "Computed state id: " << q->id << "; is final: " << q->isFinal << std::endl << std::endl;

	s->addFilter(charBitset, q);

	return q;
}


DetState* DetManager :: getNextDetState(DetState* &s , char a, size_t idx) {

	DetState *q;
	BitsetWrapper charBitset = applyFilters(a);


	q = s->nextState(charBitset);
	if(q != nullptr) {
		return q;
	}

	// Compute determinization for that transition
	q = getNextSubset(s->ss, charBitset);

	if((int)a > 0 && (int)a < 128) {
		s->addFilter(a, q); // Add to ASCII cache
	}
	else {
		// std::cout << "Adding to filters : '" << a << "\' at pos: " << idx << '\n';
		s->addFilter(charBitset, q); // Add to normal transition
	}

	return q;


}

DetState* DetManager ::getNextDetState(DetState* s, char a) {
	DetState *q;
	BitsetWrapper charBitset = applyFilters(a);


	q = s->nextState(charBitset);
	if(q != nullptr) {
		return q;
	}

	// Compute determinization for that transition
	q = getNextSubset(s->ss, charBitset);

	if((int)a > 0 && (int)a < 128) {
		s->addFilter(a, q); // Add to ASCII cache
	}
	else {
		s->addFilter(charBitset, q); // Add to normal transition
	}

	return q;
}

BitsetWrapper DetManager :: applyFilters(char a) {
	return filter_factory_->applyFilters(a);
}
