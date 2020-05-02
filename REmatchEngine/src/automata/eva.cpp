#include <queue>
#include <unordered_set>

#include "eva.hpp"
#include "lva.hpp"
#include "factories.hpp"
#include "lvastate.hpp"
#include "opt/crossprod.hpp"

ExtendedVA :: ExtendedVA(LogicalVA &A):
	vFact(A.vFact), fFact(A.fFact), currentID(0) {
	/* Constructs an extended-VA from a logical-VA */

	// Get rid off e-transitions

	epsilonClosure(A);

	adaptReachableStates(A);

	captureClosure();

	cleanUselessCaptureStates();

	cleanUselessCaptureTransitions();

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
		from->addCapture(capture.code, capture.next);
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
		if(!capture.next->tempMark)
			pruneDFS(capture.next, tmp);
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
		if(!capture.next->tempMark)
			utilCleanUnreachable(capture.next);
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
			for(auto &capture2: capture1.next->c) {
				// TODO: Check if transition already exists
				// std::cout << "\t\tState " << capture1.next->id << " has a capture pointing to " << capture2.next->id << " with code " << vFact->getVarUtil(capture2.code) << std::endl;

				newCode = (capture1.code | capture2.code);
				itState->addCapture(newCode, capture2.next);

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
				capture.from->c.erase(capture);
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
		if(!capture.next->tempMark) {
			invTopologicalSortUtil(capture.next, Q);
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
		if(!capture.next->tempMark)
			utilRelabelStates(capture.next);
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
      S = capture.code;

      nid = capture.next->id;

      ss << "t " << cid << " " << vFact->getVarUtil(S) << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture.next);
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