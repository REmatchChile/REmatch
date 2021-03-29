#include <queue>
#include <unordered_set>

#include "eva.hpp"
#include "lva.hpp"
#include "factories/factories.hpp"
#include "lvastate.hpp"
#include "opt/crossprod.hpp"

ExtendedVA :: ExtendedVA(LogicalVA &A):
	variable_factory_(A.varFactory()), filter_factory_(A.filterFactory()), currentID(0) {
	/* Constructs an extended-VA from a logical-VA */

	// Get rid off e-transitions

	// std::cout << "LVA:\n" << A.pprint() << "\n\n";

	epsilonClosure(A);

	adaptReachableStates(A);

	// std::cout << "EvA before:\n" << pprint() << "\n\n";

	#ifndef NOPT_OFFSET
	offsetOpt();
	#endif

	pruneUselessStates();

	// std::cout << "EvA after:\n" << pprint() << "\n\n";

	captureClosure();

	// TODO: Revisar si es factible hacer offset acá, con tal de correr CaptureClosure nuevamente

	// std::cout << pprint() << "\n\n";

	cleanUselessCaptureStates();

	cleanUselessCaptureTransitions();

	// std::cout << "EvA afterer:\n" << pprint() << "\n\n";

  #ifndef NOPT_CROSSPROD
	  crossProdOpt(*this);
  #endif

	relabelStates();

	searchSuperFinals();

	// std::cout << "EvA aftererer:\n" << pprint() << "\n\n";

}


ExtendedVA :: ExtendedVA():
	variable_factory_(new VariableFactory()), filter_factory_(new FilterFactory()) {

		init_state_ = new LVAState();
		states.push_back(init_state_);

}

ExtendedVA::ExtendedVA(const ExtendedVA &A)
	: idMap(A.idMap),
		variable_factory_(A.varFactory()),
		filter_factory_(A.filterFactory()),
		currentID(0) {

	// Mantain a map from the original states to their copies
	std::map<LVAState*, LVAState*> copy_table;
	LVAState *new_state;

	// Populate the copy table
	for(const auto &state: A.states) {
		new_state = new LVAState(*state); // Copy the state in heap
		states.push_back(new_state);
		if(new_state->isFinal)
			finalStates.push_back(new_state);
		if(new_state->isSuperFinal)
			superFinalStates.push_back(new_state);
		copy_table[state] = new_state;
	}

	// Update init state
	init_state_ = copy_table[A.initState()];

	// Update transition table
	for(const auto &state: this->states) {
		for(auto capture: state->c)
			capture->reset_states(copy_table[capture->from], copy_table[capture->next]);
		for(auto epsilon: state->e)
			epsilon->reset_states(copy_table[epsilon->next]);
		for(auto filter: state->f)
			filter->reset_states(copy_table[filter->next]);
	}
}

void ExtendedVA :: addFilter(LVAState* state, CharClass cs, LVAState* next) {
	filter_factory_->addFilter(cs);
	state->addFilter(filter_factory_->getCode(cs), next);
}

void ExtendedVA :: addCapture(LVAState* state, std::bitset<32> bs, LVAState* next) {
	state->addCapture(bs, next);
}


size_t ExtendedVA :: size() {
	return states.size();
}


void ExtendedVA :: epsilonClosure(LogicalVA &A) {
	/* Adds correct transitions to replace e-transitions*/

	for(auto &state: A.states) {
		state->visitedBy = -1;
	}

	for(auto &state: A.states) {
		state->visitedBy = state->id;
		for(auto &epsilon: state->e) {
			utilEpsilonClosure(state, epsilon->next);
		}
	}

}


void ExtendedVA :: utilEpsilonClosure(LVAState *from, LVAState *current) {
	current->visitedBy = from->id;
	if(current->isFinal) {
		from->isFinal = true;
	}
	for(auto &capture: current->c) {
		from->addCapture(capture->code, capture->next);
	}
	for(auto &filter: current->f) {
		from->addFilter(filter->code, filter->next);
	}

	for(auto &epsilon: current->e) {
		if(epsilon->next->visitedBy != from->id) {
			utilEpsilonClosure(from, epsilon->next);
		}

	}
}

void ExtendedVA :: adaptReachableStates(LogicalVA &A) {
	for(auto &state: A.states) {
		state->tempMark = false;
		state->incidentCaptures.clear();
		state->incidentFilters.clear();
	}

	states.reserve(A.states.size());
	finalStates.reserve(A.finalStates.size());

	init_state_ = A.initState();
	init_state_->isInit = true;

	utilCleanUnreachable(A.initState());
}

void ExtendedVA :: pruneUselessStates() {
	for(auto &state: states) {
		state->tempMark = false;
	}

	std::vector<LVAState*> tmp;
	pruneDFS(init_state_, tmp);
	states = std::move(tmp);
	finalStates.clear();
	for(auto const &state: states) {
		if(state->isFinal)
			finalStates.push_back(state);
	}

}

void ExtendedVA :: pruneDFS(LVAState *state, std::vector<LVAState*> &tmp) {
	state->tempMark = true;

	for(auto &capture: state->c) {
		if(!capture->next->tempMark)
			pruneDFS(capture->next, tmp);
	}
	for(auto &filter: state->f) {
		if(!filter->next->tempMark)
			pruneDFS(filter->next, tmp);
	}

	tmp.push_back(state);
}

void ExtendedVA :: utilCleanUnreachable(LVAState *state) {
	state->tempMark = true;

	for(auto &capture: state->c) {
		capture->next->incidentCaptures.push_back(capture);
		if(!capture->next->tempMark)
			utilCleanUnreachable(capture->next);
	}
	for(auto &filter: state->f) {
		filter->next->incidentFilters.push_back(filter);
		if(!filter->next->tempMark)
			utilCleanUnreachable(filter->next);
	}

	states.push_back(state);
	if(state->isFinal) {
		finalStates.push_back(state);
	}

}

void ExtendedVA :: captureClosure() {
	/* Using the modified inverse topological order of the automaton capture states
	computes the capture closure of it */

	// Get the modified topological sort of the e-VA
	std::queue<LVAState*> topOrder = invTopologicalSort();


	LVAState* itState; // Iterator state

	while(!topOrder.empty()) {
		itState = topOrder.front();
		topOrder.pop();

		std::bitset<32> newCode;

		std::vector<LVACapture*>::iterator it1, it2;

		for(auto &capture1: itState->c) {
			for(auto &capture2: capture1->next->c) {
				newCode = (capture1->code | capture2->code);
				itState->addCapture(newCode, capture2->next);
			}
		}
	} // while(!topOrder.empty())

}

void ExtendedVA :: cleanUselessCaptureStates() {

	bool isUselessCaptureState;
	// Iterate through every state in the automaton
	for(auto state=states.begin(); state != states.end(); ) {

		isUselessCaptureState = ((*state)->incidentFilters.empty() &&
														(*state)->f.empty() &&
														!(*state)->isFinal &&
														!(*state)->isInit);

		if(isUselessCaptureState) {
			// Remove the incident capture transitions on previous states
			for(auto &capture: (*state)->incidentCaptures) {
				for(auto it=capture->from->c.begin(); it != capture->from->c.end(); ) {
					if(capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->from->c.erase(it);
					else
						++it;
				}
			}
			// Remove the state from the list of states
			state = states.erase(state);
		}
		else {
			++state;
		}
	}
}

void ExtendedVA :: cleanUselessCaptureTransitions() {
	for(auto &state: states) {
		if(state->incidentFilters.empty() && !state->incidentCaptures.empty()) {
			state->c.clear();
		}
	}
}

void ExtendedVA::offsetOpt() {
	// Vector of lists of capture transitions with the same code
	std::vector<CaptureList> classifier = classifySingleCaptures();

	// Iterate through captures in inverse topological order
	for(auto &capture: getInvTopSortCaptures()) {
		// Search the corresponding list in the classifier
		for(size_t i=0; i < 2*variable_factory_->size(); i++) {
			if(capture->code[i]) {
				computeOffset(classifier[i], i); // Compute offset in bulk
				break;
			}
		}
	}
}

void ExtendedVA::computeOffset(CaptureList &captureList, int codeIndex) {
	std::shared_ptr<LVAFilter> filter;
	CapturePtr capture;
	LVAState *p, *q, *r;

	while(offsetPossible(captureList)) {
		for(auto it = captureList.begin(); it != captureList.end();) {
			capture = *it;
			p = capture->from; q = capture->next;

			for(auto &filter: q->f) {
				r = filter->next;

				// Create new state
				LVAState* q_prim = new LVAState();
				states.push_back(q_prim);

				filter->next = q_prim;
				q_prim->addCapture(capture->code, r);

				captureList.push_front(q_prim->c.front());
				p->addFilter(filter->code, q_prim);

				if(q->incidentFilters.empty())
					r->incidentFilters.remove(filter);
			}

			p->c.remove(capture);
			q->incidentCaptures.remove(capture);

			p->addEpsilon(q);

			it = captureList.erase(it);

		}

		variable_factory_->getOffset(codeIndex)++;
	}
}

bool ExtendedVA::offsetPossible(CaptureList &captureList) {
	for(auto &capture: captureList) {
		if(!offsetPossible(capture))
			return false;
	}
	return true;
}

bool ExtendedVA::offsetPossible(CapturePtr capture) {
	// We want the following:
	// (q) ---[capture]---> (p) -----[filter]----> (r)
	// So we can then change it to:
	// (q) ---[filter]----> (p) ---[capture-1]---> (r)
	// So we need to check:
	// 	1. Capture has only one variable involved.
	//  2. (p) can't be a final state.
	//	3. (p) has at least 1 filter transition and no capture transitions.

	LVAState *p;
	p = capture->next;

 	if(capture->code.count() != 1)
		return false;
	if(p->isFinal)
		return false;
	if(p->f.size() == 0 || p->c.size() > 0)
		return false;
	if(p->incidentCaptures.size() != 1)
		return false;
	for(auto &filter: p->f) {
		if(isReachable(filter->next, p))
			return false;
	}

	return true;
}

bool ExtendedVA::isReachable(LVAState *from, LVAState *end) {
	for(auto &state: states)
		state->tempMark = false;

	std::vector<LVAState*> stack;
	for(auto &filter: from->f)
		stack.push_back(filter->next);

	while(!stack.empty()) {
		LVAState* currentState = stack.back(); stack.pop_back();
		if(currentState == end)
			return true;
		if(currentState->tempMark)
			continue;
		currentState->tempMark = true;
		for(auto &filter: currentState->f)
			stack.push_back(filter->next);
	}

	return false;
}

std::vector<CaptureList> ExtendedVA::classifySingleCaptures() {
	std::vector<CaptureList> classifier;

	classifier.resize(2*variable_factory_->size());

	for(auto &state: states) {
		for(auto &capture: state->c) {
			if(capture->code.count() == 1) { // If single variable capture

				// Search for the variable code position and store the capture at
				// that position in the classifier
				for(size_t i=0; i < 2*variable_factory_->size(); i++) {
					if(capture->code[i]) {
						classifier[i].push_back(capture);
						break;
					}
				}
			}
		}
	}

	return classifier;
}





// It's assumed that there is no epsilon transitions anymore.
CaptureVector ExtendedVA::getInvTopSortCaptures() {

	CaptureVector totCaptures;
	// Unset flag for each capture and push capture to container.
	for(auto &state: states) {
		for(auto &capture: state->c) {
			capture->flag = false;
			totCaptures.push_back(capture);
		}
	}

	CaptureVector L; // Returning container
	CapturePtr currCap;

	if(totCaptures.empty()) return L;

	do {
		currCap = totCaptures.back(); totCaptures.pop_back();
		if(!currCap->flag)
			getInvTopSortCapturesUtil(currCap, L);

	} while(!totCaptures.empty());

	return L;
}

void ExtendedVA::getInvTopSortCapturesUtil(CapturePtr &cap, CaptureVector &L) {
	if(cap->flag)
		return;

	cap->flag = true;

	for(auto &reachedCap: reachableCaptures(cap)) {
		getInvTopSortCapturesUtil(reachedCap, L);
	}

	L.push_back(cap);
}

CaptureVector ExtendedVA::reachableCaptures(CapturePtr &cap) {
	for(auto &state: states)
		state->tempMark = false;

	std::vector<LVAState*> stack;
	stack.push_back(cap->next);

	CaptureVector ret;

	LVAState* s;

	while(!stack.empty()) {
		s = stack.back() ; stack.pop_back();
		s->tempMark = true;
		for(auto &capture: s->c) {
			ret.push_back(capture);
		}
		for(auto &filter: s->f) {
			if(!filter->next->tempMark) {
				stack.push_back(filter->next);
			}
		}
	}

	return ret;
}


std::queue<LVAState*> ExtendedVA :: invTopologicalSort() {
	std::queue<LVAState*>* Q = new std::queue<LVAState*>();

	for(auto &state: states) {
		state->tempMark = false;
	}

	for(auto &state: states) {
		if(!state->tempMark) {
			invTopologicalSortUtil(state, Q);
		}
	}

	return *Q;
}

void ExtendedVA :: invTopologicalSortUtil(LVAState *state, std::queue<LVAState*> *Q) {
	state->tempMark = true;

	if(state->c.empty()) return; // Not interested if no captures present

	for(auto &capture: state->c) {
		if(!capture->next->tempMark) {
			invTopologicalSortUtil(capture->next, Q);
		}
	}

	Q->push(state);
}

void ExtendedVA :: relabelStates() {
	for(auto &state: states) {
		state->tempMark = false;
	}

	idMap.clear();

	utilRelabelStates(init_state_);
}

void ExtendedVA :: utilRelabelStates(LVAState *state) {
	state->tempMark = true;
	state->id = currentID;
	idMap[state->id] = state;
	currentID++;

	for(auto &capture: state->c) {
		if(!capture->next->tempMark)
			utilRelabelStates(capture->next);
	}
	for(auto &filter: state->f) {
		if(!filter->next->tempMark)
			utilRelabelStates(filter->next);
	}
}


std::string ExtendedVA :: pprint() {
  /* Gives a codification for the LogicalVA that can be used to visualize it
     at https://puc-iic2223.github.io . Basically it uses Breath-First Search
     to get every labeled transition in the ExtendedVA with the unique ids for
     each LVAState */


  // Declarations
  std::stringstream ss, cond;
  LVAState *current;
  int cid, nid;  // cid: current LVAState id; nid : next LVAState id
  std::bitset<32> S;



  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<LVAState*> queue;

  // Start on the init LVAState
  visited.insert(init_state_->id);
  queue.push_back(init_state_);



  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();
    cid = current->id;

    // For every capture transition
    for (auto &capture: current->c) {
      S = capture->code;

      nid = capture->next->id;

      ss << "t " << cid << " " << variable_factory_->getVarUtil(S) << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture->next);
      }
    }

    // For every filter transition
    for (auto &filter: current->f) {
      nid = filter->next->id;
      S = filter->code;

      ss << "t " << cid << ' ' << filter_factory_->getFilter(filter->code).print() << ' ' << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(filter->next);
      }
    }
  }

  // Code final states
  for (size_t i = 0; i < finalStates.size(); ++i) {
    if(finalStates[i]->isFinal) {
      ss << "f " << finalStates[i]->id << '\n';
    }
  }

  // Code initial LVAState
  ss << "i " << init_state_->id;

  return ss.str();
}

void ExtendedVA :: searchSuperFinals() {
	for(auto &fState: finalStates) {
		for(auto &state: states) {
			state->colorMark = 'w'; // Mark as white
		}
		if(!fState->isSuperFinal) {
			utilSearchSuperFinals(fState);
		}
	}
}

bool ExtendedVA :: utilSearchSuperFinals(LVAState *s) {
	s->colorMark = 'g'; // Mark as grey

	for(auto &filter: s->f) {
		if(filter_factory_->getFilter(filter->code).label == "." && filter->next->isFinal) {
			if(filter->next->colorMark == 'g' || filter->next->isSuperFinal) {
				s->isSuperFinal = true;
				superFinalStates.push_back(s);
				return true;
			}
			else if(filter->next->colorMark == 'w') {
				if(utilSearchSuperFinals(filter->next)) {
					s->isSuperFinal = true;
					superFinalStates.push_back(s);
					return true;
				}
			}
		}
	}
	s->colorMark = 'b'; // Mark as black
	return false;
}

std::set<LVAState*> ExtendedVA :: getSubset(BitsetWrapper bs) const {
	std::set<LVAState*> ret;
	for(size_t i=0; i < bs.size(); i++)
		if(bs.get(i))
			ret.insert(idMap.at(i));
	return ret;
}