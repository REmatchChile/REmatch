#include <queue>
#include <unordered_set>

#include "eva.hpp"
#include "lva.hpp"
#include "factories/factories.hpp"
#include "lvastate.hpp"
#include "opt/crossprod.hpp"

ExtendedVA :: ExtendedVA(LogicalVA &A):
	vFact(A.vFact), fFact(A.fFact), currentID(0) {
	/* Constructs an extended-VA from a logical-VA */

	// Get rid off e-transitions

	epsilonClosure(A);

	adaptReachableStates(A);

	// std::cout << pprint() << "\n\n";

	// offsetOpt();

	// std::cout << "EvA after:\n" << pprint() << "\n\n";

	captureClosure();

	// TODO: Revisar si es factible hacer offset acá, con tal de correr CaptureClosure nuevamente

	// std::cout << pprint() << "\n\n";

	cleanUselessCaptureStates();

	cleanUselessCaptureTransitions();

	// std::cout << pprint() << "\n\n";

  #ifndef NOPT_CROSSPROD
	  crossProdOpt(*this);
  #endif

	relabelStates();

	searchSuperFinals();

}


ExtendedVA :: ExtendedVA():
	vFact(new VariableFactory()), fFact(new FilterFactory) {

		initState = new LVAState();
		states.push_back(initState);

}

ExtendedVA::ExtendedVA(const ExtendedVA &A)
	: idMap(A.idMap),
		vFact(A.vFact),
		fFact(A.fFact),
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
	initState = copy_table[A.initState];

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
	fFact->addFilter(cs);
	state->addFilter(fFact->getCode(cs), next);
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
		// TODO: Check if the transition already exists
		from->addCapture(capture->code, capture->next);
	}
	for(auto &filter: current->f) {
		// TODO: Check if the transition already exists
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
	}

	states.reserve(A.states.size());
	finalStates.reserve(A.finalStates.size());

	initState = A.initState;
	initState->isInit = true;

	utilCleanUnreachable(A.initState);
}

void ExtendedVA :: pruneUselessStates() {
	for(auto &state: states) {
		state->tempMark = false;
	}

	std::vector<LVAState*> tmp;
	pruneDFS(initState, tmp);
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
		if(!capture->next->tempMark)
			utilCleanUnreachable(capture->next);
	}
	for(auto &filter: state->f) {
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

		// std::cout << "Currently at state " << itState->id << " at queue.\n";

		std::bitset<32> newCode;

		std::vector<LVACapture*>::iterator it1, it2;

		for(auto &capture1: itState->c) {
			// std::cout << "\tState " << itState->id << " has a capture pointing to " << capture1.next->id << " with code " << vFact->getVarUtil(capture1.code) << std::endl;
			for(auto &capture2: capture1->next->c) {
				// TODO: Check if transition already exists
				// std::cout << "\t\tState " << capture1.next->id << " has a capture pointing to " << capture2.next->id << " with code " << vFact->getVarUtil(capture2.code) << std::endl;

				newCode = (capture1->code | capture2->code);
				itState->addCapture(newCode, capture2->next);

				// std::cout << "\t\tState " << itState->id << " connected to " << capture2.next->id << " by capture with code " << vFact->getVarUtil(newCode) << std::endl;
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
	std::vector<std::list<std::shared_ptr<LVACapture>>> classifier = classifySingleCaptures();

	// Iterate through captures in topological order from the final states
	for(auto &capture: getInvTopSortCaptures()) {

		// Search the corresponding list in the classifier
		for(size_t i=0; i < 2*vFact->size(); i++) {
			if(capture->code[i]) {
				computeOffset(classifier[i], i); // Compute offset in bulk
				break;
			}
		}
	}
}

void ExtendedVA::computeOffset(std::list<std::shared_ptr<LVACapture>> &captureList, int codeIndex) {
	std::shared_ptr<LVAFilter> filter;
	std::shared_ptr<LVACapture> capture;
	LVAState *p, *q, *r;

	while(offsetPossible(captureList)) {
		for(auto it = captureList.begin(); it != captureList.end();) {
			capture = *it;
			p = capture->from; q = capture->next;
			filter = q->f.front(); // Front exists because offset is possible
			r = filter->next;

			auto captureCopy = *capture;
			auto filterCopy = *filter;

			// // Transformation
			// p->c.remove(captureCopy);
			// q->f.remove(filterCopy);

			// q->incidentCaptures.remove(captureCopy); // Destroys the pointed capture
			// r->incidentFilters.remove(filterCopy);

			p->addFilter(filterCopy.code, q);
			q->addCapture(captureCopy.code, r);

			it = captureList.erase(it);
			captureList.insert(it, q->c.back());
		}

		vFact->getOffset(codeIndex)++;
	}
}

bool ExtendedVA::offsetPossible(std::list<std::shared_ptr<LVACapture>> &captureList) {
	for(auto &capture: captureList) {
		if(!offsetPossible(capture))
			return false;
	}
	return true;
}

bool ExtendedVA::offsetPossible(std::shared_ptr<LVACapture> capture) {
	// We want the following:
	// (q) ---[capture]---> (p) -----[filter]----> (r)
	// So we can then change it to:
	// (q) ---[filter]----> (p) ---[capture-1]---> (r)
	// So we need to check:
	// 	1. Capture has only one variable involved.
	//  2. (p) can't be a final state.
	//	3. (p) has 1, and only 1 filter transition.
	//  4. If (p) has only one capture and no filter transition, then ask
	//     to the next capture.

 	if(capture->code.count() != 1)
		return false;
	if(capture->next->isFinal)
		return false;
	// if(capture->next->c.size() == 1 && capture->next->f.empty())
	// 	return offsetPossible(&capture->next->c.front());
	if(capture->next->f.size() != 1 )
		return false;
	if(capture->next->c.empty())
		return false;
	// TODO: (p) debería tener solo una transicion entrando (capture)

	return true;
}

std::vector<std::list<std::shared_ptr<LVACapture>>> ExtendedVA::classifySingleCaptures() {
	std::vector<std::list<std::shared_ptr<LVACapture>>> classifier;

	classifier.resize(2*vFact->size());

	for(auto &state: states) {
		for(auto &capture: state->c) {
			if(capture->code.count() == 1) { // If single variable capture

				// Search for the variable code position and store the capture at
				// that position in the classifier
				for(size_t i=0; i < 2*vFact->size(); i++) {
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
std::vector<std::shared_ptr<LVACapture>> ExtendedVA::getInvTopSortCaptures() {
	// Unset flag for each state.
	for(auto &state: states) {
		state->tempMark = false;
	}
	std::vector<LVAState*> stack; // DFS stack
	std::vector<std::shared_ptr<LVACapture>> captureStack; // Returning stack
	for(auto &fState: finalStates) {
		fState->tempMark = true;
		stack.push_back(fState);
	}

	LVAState *currentState;

	while(!stack.empty()) {
		currentState = stack.back(); stack.pop_back();

		for(auto &capture: currentState->incidentCaptures) {
			if(capture->code.count() == 1) // Single variable opened/closed
				captureStack.push_back(capture);
			if(!capture->from->tempMark) {
				capture->from->tempMark = true;
				stack.push_back(capture->from);
			}
		}
		for(auto &filter: currentState->incidentFilters) {
			if(!filter->from->tempMark) {
				filter->from->tempMark = true;
				stack.push_back(filter->from);
			}
		}
	}

	return captureStack;
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

	utilRelabelStates(initState);
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
  visited.insert(initState->id);
  queue.push_back(initState);



  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();
    cid = current->id;

    // For every capture transition
    for (auto &capture: current->c) {
      S = capture->code;

      nid = capture->next->id;

      ss << "t " << cid << " " << vFact->getVarUtil(S) << " " << nid << '\n';

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

      // TODO: Get the correct transition name

      // ss << "t " << cid << ' ' << fFact->getFilter(current->f[i]->code).print() << ' ' << nid << '\n';
      ss << "t " << cid << ' ' << fFact->getFilter(filter->code).print() << ' ' << nid << '\n';

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
  ss << "i " << initState->id;

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
		if(fFact->getFilter(filter->code).label == "." && filter->next->isFinal) {
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