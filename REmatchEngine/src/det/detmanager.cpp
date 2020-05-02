#include "detmanager.hpp"
#include "automata/detautomaton.hpp"
#include "automata/lva.hpp"
#include "automata/eva.hpp"
#include "automata/detstate.hpp"
#include "automata/lvastate.hpp"
#include "factories.hpp"
#include "setstate.hpp"
#include "structs/vector.hpp"
#include "captures.hpp"
#include "automata/eva.hpp"

#include "timer.hpp"

#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>
#include <stack>
#include <random>

#ifndef __EMSCRIPTEN__

#include <boost/serialization/nvp.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

using namespace boost::multiprecision;

#endif

DetManager :: DetManager(ExtendedVA *A,
						 DetAutomaton *detA,
						 std::array<rematch::vector<DetState*>*, 3> vectors):
	eVA(A),
	detA(detA),
	dependentVectors(vectors),
	numDetStatesThreshold(A->states.size()*2),
  vector_connection(true)

{
	// Set init reserve space for dependent vectors
	for(auto &vect: dependentVectors) {
		vect->reserve(numDetStatesThreshold);
	}

	std::set<LVAState*> newSubset;
	newSubset.insert(eVA->initState);

	SetState* ss = new SetState(eVA, newSubset);
	DetState *q = detA->initState;
	q->setSubset(ss);

	setStatesMap[ss->bitstring] = q;

	if(q->isFinal) {
		detA->finalStates.push_back(q);
	}

	computeCaptures(q);
}

DetManager :: DetManager(ExtendedVA *A, DetAutomaton *detA):
	eVA(A), detA(detA), vector_connection(false)
{

	std::set<LVAState*> newSubset;
	newSubset.insert(eVA->initState);

	SetState* ss = new SetState(eVA, newSubset);
	DetState *q = detA->initState;
	q->setSubset(ss);

  allCharBitsets = eVA->fFact->allPossibleCharBitsets();

	setStatesMap[ss->bitstring] = q;

	if(q->isFinal) {
		detA->finalStates.push_back(q);
	}

	computeCaptures(q);
}


void DetManager :: computeFullDetAutomaton() {
  // Timer t;
	std::stack<DetState*> stateList;
	stateList.push(detA->initState);
	DetState* currState;
	size_t numFilters = eVA->fFact->size();
	size_t numStates = eVA->size();

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

		for(auto &chrbset: allCharBitsets) {
			newSubsetBitset.clear();
			newSubsetBitset.resize(numStates);
			for(auto &keyValue : nextSubsetMap) {
				if(chrbset.first.get(keyValue.first))
					newSubsetBitset |= keyValue.second;
			}

			// std::cout << "Charbset: " << chrbset << "\n";

			// Look for DetState in hashtable
			auto found = setStatesMap.find(newSubsetBitset);

			// If not found add to hashtable
			if (found == setStatesMap.end()) {
				SetState *ss = new SetState(eVA, eVA->getSubset(newSubsetBitset));
				DetState *nq = new DetState(ss);

				found = setStatesMap.insert({ss->bitstring, nq}).first;

				detA->states.push_back(nq);

				if(nq->isFinal)
					detA->finalStates.push_back(nq);

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
  std::vector<char> &intersection = allCharBitsets[bs];
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, intersection.size()-1);

	return intersection[dis(gen)];

}

#ifndef __EMSCRIPTEN__

std::string DetManager :: uniformSample(size_t n) {
  // Timer t;
	std::unordered_map<DetState*, std::pair<std::vector<BitsetWrapper>, cpp_int>> T, Tprim;
	T.insert({detA->initState, std::make_pair(std::vector<BitsetWrapper>(), 1)});
	DetState *q, *p;
	const BitsetWrapper *a;
	std::vector<BitsetWrapper> *w, *u;
	cpp_int N, M, S;

  cpp_bin_float_quad prob;

	double c;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	detA->computeOneReached();

  // std::cout << "Init algo: " << t.elapsed() << '\n';
  // t.reset();

	for(size_t i=1; i <= n; i++) {
		for(auto &state_pair: T) {
			p = state_pair.first; w = &state_pair.second.first; N = state_pair.second.second;
			for(auto &bset_state: p->oneReached) {
				a = &bset_state.first; q = bset_state.second;
        S = allCharBitsets[*a].size();
				if(!Tprim.count(q)) {
					std::vector<BitsetWrapper> wa = *w;
					wa.push_back(*a);
					Tprim[q] = std::make_pair(wa, N*S);
				}
				else {
					u = &Tprim[q].first; M = Tprim[q].second;
					c = dis(gen); prob = cpp_bin_float_quad(M)/cpp_bin_float_quad(M+N*S);
					if ( c <= prob) {
						std::vector<BitsetWrapper> ua = *u;
						Tprim[q] = std::make_pair(ua, M+N*S);
					}
					else {
						std::vector<BitsetWrapper> wa = *w;
						wa.push_back(*a);
						Tprim[q] = std::make_pair(wa, M+N*S);
					}
				}
			}
		}

		T = Tprim;
		Tprim.clear();
	}

  // std::cout << "Populate hash tables: " << t.elapsed() << '\n';
  // t.reset();

	M = 0; u = nullptr;
	for(auto &state_pair: T) {
		p = state_pair.first; w = &state_pair.second.first; N = state_pair.second.second;
		if(p->isFinal) {
			c = dis(gen); prob = cpp_bin_float_quad(N)/cpp_bin_float_quad(M+N);
			if (c <= prob)
				u = w;
			M += N;
		}
	}

  // std::cout << "Get bitset string: " << t.elapsed() << '\n';
  // t.reset();

	std::stringstream ss;
	char d;
	if(u != nullptr) {
		for(auto &bs: *u) {
			ss << chooseFromCharBitset(bs);
		}
	}

  // std::cout << "Get full string: " << t.elapsed() << '\n';
  // t.reset();

	return ss.str();
}

#endif

DetState* DetManager :: getNextSubset(SetState* ss, BitsetWrapper charBitset) {
	/* Gets next subset from a subset ss if filter bitset b is read */

	std::set<LVAState*> newSubset;  // Store the next subset
	BitsetWrapper subsetBitset(eVA->size());  // Subset bitset representation

	for(auto &state: ss->subset) {
		for(auto &filter: state->f) {
			if(charBitset.get(filter->code) && !subsetBitset.get(filter->next->id)) {
				// std::cout << "\t\tFilter code " << filter->code << " triggered at state " << state->id << std::endl;
				newSubset.insert(filter->next);
				subsetBitset.set(filter->next->id, true);
			}
		}
	}

	auto found = setStatesMap.find(subsetBitset);

	if(found == setStatesMap.end()) { // Check if already stored subset
		SetState* ss = new SetState(eVA, newSubset);
		DetState* nq = new DetState(ss);

		setStatesMap[ss->bitstring] = nq;

		detA->states.push_back(nq);

		if(nq->isFinal) {
			detA->finalStates.push_back(nq);
		}

		computeCaptures(nq);
	}

	return setStatesMap[subsetBitset];
}


void DetManager :: computeCaptures(DetState* q) {
	/* Computes the reachable subsets from the deterministic state q through
	captures, stores them if necessary and connects q to the computed
	deterministic states thourgh deterministic capture transitions */

	std::unordered_map<std::bitset<32>, std::set<LVAState*>> captureList;
	std::unordered_map<std::bitset<32>, std::set<LVAState*>>::iterator it;

	for(auto &extState: q->ss->subset) {
		for(auto &capture: extState->c) {

			it = captureList.find(capture.code);


			if(it == captureList.end()) {
				it = captureList.emplace(capture.code, std::set<LVAState*>()).first;
			}
			it->second.insert(capture.next);
		}
	}



	for(std::pair<std::bitset<32>, std::set<LVAState*>> el: captureList){
		SetState* nss = new SetState(eVA, el.second);

		/* Check if subset is not on hash table */
		if(!setStatesMap.count(nss->bitstring)) {
			DetState* nq = new DetState(nss);
			setStatesMap[nss->bitstring] = nq;

			detA->states.push_back(nq);

			if (nq->isFinal) {
				detA->finalStates.push_back(nq);
			}

		}

		// TODO: Delete nss if subset was already on hash table

		DetState* p = setStatesMap[nss->bitstring]; // This is the deterministic state where the capture reaches

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

	// Need to check if current number of DetStates
  if(vector_connection) {
    if(detA->states.size() == numDetStatesThreshold) {
		numDetStatesThreshold *= 2;
		for(auto &vect: dependentVectors) {
			vect->reserve(numDetStatesThreshold);
		}

		// IMPORTANT! dependentVectors[0] needs to be Evaluation::currentStates
		s = (*dependentVectors[0])[idx];
	  }
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
	return eVA->fFact->applyFilters(a);
}
