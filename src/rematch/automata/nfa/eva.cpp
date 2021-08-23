#include <queue>
#include <stack>
#include <unordered_set>
#include <algorithm>

#include "eva.hpp"
#include "lva.hpp"
#include "factories/factories.hpp"
#include "automata/nfa/state.hpp"

namespace rematch {

ExtendedVA :: ExtendedVA(LogicalVA &A)
		:	variable_factory_(A.varFactory()),
			filter_factory_(A.filterFactory()),
			currentID(0),
			is_raw_(A.is_raw_) {
	/* Constructs an extended-VA from a logical-VA */

	// Get rid off e-transitions

	// if(!is_raw_)
	// 	std::cout << "LVA:\n" << A.pprint() << "\n\n";

	if(!is_raw_) {
		State* s = A.new_state();
		s->addFilter(filter_factory_->addFilter(CharClass('\0')), A.init_state_);
		A.init_state_ = s;
	}

	epsilonClosure(A);

	adaptReachableStates(A);

	// if(!is_raw_)
	// 	std::cout << "EvA before:\n" << pprint() << "\n\n";

	compute_if_dfa_searchable();

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
	if(!is_raw_)
	  crossProdOpt();
  #endif

	relabelStates();

	searchSuperFinals();

	// if(!is_raw_)
	// 	std::cout << "EvA aftererer:\n" << pprint() << "\n\n";
}

ExtendedVA :: ExtendedVA():
	variable_factory_(new VariableFactory()), filter_factory_(new FilterFactory()) {

		init_state_ = new State();
		states.push_back(init_state_);
}

ExtendedVA::ExtendedVA(const ExtendedVA &A)
	: idMap(A.idMap),
		variable_factory_(A.varFactory()),
		filter_factory_(A.filterFactory()),
		currentID(0) {

	// Mantain a map from the original states to their copies
	std::map<State*, State*> copy_table;
	State *new_state;

	// Populate the copy table
	for(const auto &state: A.states) {
		new_state = new State(*state); // Copy the state in heap
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
		for(auto capture: state->captures)
			capture->reset_states(copy_table[capture->from], copy_table[capture->next]);
		for(auto epsilon: state->epsilons)
			epsilon->reset_states(copy_table[epsilon->next]);
		for(auto filter: state->filters)
			filter->reset_states(copy_table[filter->next]);
	}
}

void ExtendedVA :: addFilter(State* state, CharClass cs, State* next) {
	filter_factory_->addFilter(cs);
	state->addFilter(filter_factory_->getCode(cs), next);
}

void ExtendedVA :: addCapture(State* state, std::bitset<32> bs, State* next) {
	state->addCapture(bs, next);
}


size_t ExtendedVA :: size() {
	return states.size();
}


void ExtendedVA::epsilonClosure(LogicalVA &A) {
	/* Adds correct transitions to replace e-transitions*/

	for(auto &state: A.states) {
		state->visitedBy = -1;
	}

	for(auto &root_state: A.states) {
		root_state->visitedBy = root_state->id;

		std::stack<State*> stack;

		for(auto &epsilon: root_state->epsilons) {
			stack.push(epsilon->next);
		}

		while(!stack.empty()) {
			State* cstate = stack.top(); stack.pop();
			if(cstate->isFinal)
				root_state->isFinal = true;
			for(auto &capture: cstate->captures)
				root_state->addCapture(capture->code, capture->next);

			for(auto &filter: cstate->filters) {
				root_state->addFilter(filter->code, filter->next);
				if(is_raw_ && (root_state->flags_ & State::kPreCaptureState)) {
					filter->next->flags_ |= State::kCaptureState;
				}
			}

			for(auto &epsilon: cstate->epsilons) {
				if(epsilon->next->visitedBy != root_state->id)
					stack.push(epsilon->next);
			}
		} // end while
	} // end for
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

	std::vector<State*> tmp, diff;
	pruneDFS(init_state_, tmp);

	std::sort(std::begin(states), std::end(states));
	std::sort(std::begin(tmp), std::end(tmp));

	std::set_difference(states.begin(), states.end(), tmp.begin(), tmp.end(),
		std::inserter(diff, diff.begin()));

	for(auto &state : diff) {
		for(auto &filter: state->filters) {
			filter->next->incidentFilters.remove(filter);
		}
		for(auto &capture: state->captures) {
			capture->next->incidentCaptures.remove(capture);
		}
	}

	states = std::move(tmp);
	finalStates.clear();
	for(auto const &state: states) {
		if(state->isFinal)
			finalStates.push_back(state);
	}

}

void ExtendedVA :: pruneDFS(State *state, std::vector<State*> &tmp) {
	state->tempMark = true;

	for(auto &capture: state->captures) {
		if(!capture->next->tempMark)
			pruneDFS(capture->next, tmp);
	}
	for(auto &filter: state->filters) {
		if(!filter->next->tempMark)
			pruneDFS(filter->next, tmp);
	}

	tmp.push_back(state);
}

void ExtendedVA :: utilCleanUnreachable(State *state) {
	state->tempMark = true;

	for(auto &capture: state->captures) {
		capture->next->incidentCaptures.push_back(capture);
		if(!capture->next->tempMark)
			utilCleanUnreachable(capture->next);
	}
	for(auto &filter: state->filters) {
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
	std::queue<State*> topOrder = invTopologicalSort();


	State* itState; // Iterator state

	while(!topOrder.empty()) {
		itState = topOrder.front();
		topOrder.pop();

		std::bitset<32> newCode;

		std::vector<LVACapture*>::iterator it1, it2;

		for(auto &capture1: itState->captures) {
			for(auto &capture2: capture1->next->captures) {
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
														(*state)->filters.empty() &&
														!(*state)->isFinal &&
														!(*state)->isInit);

		if(isUselessCaptureState) {
			// Remove the incident capture transitions on previous states
			for(auto &capture: (*state)->incidentCaptures) {
				for(auto it=capture->from->captures.begin(); it != capture->from->captures.end(); ) {
					if(capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->from->captures.erase(it);
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
			state->captures.clear();
		}
		for(auto capture = state->captures.begin(); capture != state->captures.end();) {
			if((*capture)->next->filters.empty() && !(*capture)->next->isFinal) {
				(*capture)->next->incidentCaptures.remove(*capture);
				capture = state->captures.erase(capture);
			} else {
				capture++;
			}
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
	State *p, *q, *r;

	// states p,q,r will label in the following way:
	// (p) ---[capture]---> (q) -----[filter]----> (r)

	while(offsetPossible(captureList)) {
		for(auto it = captureList.begin(); it != captureList.end();) {
			capture = *it;
			p = capture->from; q = capture->next;

			for(auto &filter: q->filters) {
				r = filter->next;

				// Create new state q'
				State* q_prim = new State();
				states.push_back(q_prim);

				filter->next->incidentFilters.remove(filter);
				filter->next = q_prim;

				// Current picture:
				// (p) --[capture]--> (q) ---[filter]---> (q') (r)

				q_prim->addCapture(capture->code, r);

				// Current picture:
				// (p) --[capture]--> (q) ---[filter]---> (q') --[capture]--> (r)

				// Need to add the new capture to the captureList as maybe it's possible
				// to keep offseting the capture in the next iteration
				captureList.push_front(q_prim->captures.front());


				p->addFilter(filter->code, q_prim);

				// Current picture:
				// (p) --[capture]--> (q) ---[filter]---> (q') --[capture]--> (r)
				//  ᴸ--------------[filter]----------------^

				if(q->incidentFilters.empty())
					r->incidentFilters.remove(filter);
			}

			p->captures.remove(capture);
			q->incidentCaptures.remove(capture);
			if(q->incidentCaptures.empty())
				q->flags_ &= ~State::kCaptureState;

			// Current picture:
			// (p)                (q) ---[filter]---> (q') --[capture]--> (r)
			//  ᴸ--------------[filter]----------------^

			// Notice that (q) will be eliminated if it's not reachable in the
			// step after the offsets are computed.

			p->addEpsilon(q);

			it = captureList.erase(it);

		}

		variable_factory_->get_offset(codeIndex)++;
	}
}

bool ExtendedVA::offsetPossible(CaptureList &captureList) {
	for(auto &capture: captureList) {
		if(!offsetPossible(capture))
			return false;
		for(auto &capture2: captureList) {
			if(capture2 == capture) continue;
			if(!offsetPossible(capture, capture2))
				return false;
		}
	}
	return true;
}

bool ExtendedVA::offsetPossible(CapturePtr capture1, CapturePtr capture2) {
	for(auto &filter: capture1->next->filters) {
		if(isReachable(filter->next, capture2->next))
			return false;
	}
	return true;
}

bool ExtendedVA::offsetPossible(CapturePtr capture) {
	// We want the following:
	// (p) ---[capture]---> (q) -----[filter]----> (r)
	// So we can then change it to:
	// (p) ---[filter]----> (q) ---[capture-1]---> (r)
	// So we need to check:
	// 	1. Capture has only one variable involved.
	//  2. (q) can't be a final state.
	//	3. (q) has at least 1 filter transition and no capture transitions.

	State *q;
	q = capture->next;

 	if(capture->code.count() != 1)
		return false;
	if(q->isFinal)
		return false;
	if(q->filters.size() == 0 || q->captures.size() > 0)
		return false;
	if(q->incidentCaptures.size() != 1)
		return false;
	for(auto &filter: q->filters) {
		if(isReachable(filter->next, q))
			return false;
	}

	return true;
}

bool ExtendedVA::isReachable(State *from, State *end) {
	for(auto &state: states)
		state->tempMark = false;

	std::vector<State*> stack;
	for(auto &filter: from->filters)
		stack.push_back(filter->next);

	while(!stack.empty()) {
		State* currentState = stack.back(); stack.pop_back();
		if(currentState == end)
			return true;
		if(currentState->tempMark)
			continue;
		currentState->tempMark = true;
		for(auto &filter: currentState->filters)
			stack.push_back(filter->next);
	}

	return false;
}

std::vector<CaptureList> ExtendedVA::classifySingleCaptures() {
	std::vector<CaptureList> classifier;

	classifier.resize(2*variable_factory_->size());

	for(auto &state: states) {
		for(auto &capture: state->captures) {
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
		for(auto &capture: state->captures) {
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

	std::vector<State*> stack;
	stack.push_back(cap->next);

	CaptureVector ret;

	State* s;

	while(!stack.empty()) {
		s = stack.back() ; stack.pop_back();
		s->tempMark = true;
		for(auto &capture: s->captures) {
			ret.push_back(capture);
		}
		for(auto &filter: s->filters) {
			if(!filter->next->tempMark) {
				stack.push_back(filter->next);
			}
		}
	}

	return ret;
}


std::queue<State*> ExtendedVA :: invTopologicalSort() {
	std::queue<State*>* Q = new std::queue<State*>();

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

void ExtendedVA :: invTopologicalSortUtil(State *state, std::queue<State*> *Q) {
	state->tempMark = true;

	if(state->captures.empty()) return; // Not interested if no captures present

	for(auto &capture: state->captures) {
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

void ExtendedVA :: utilRelabelStates(State *state) {
	state->tempMark = true;
	state->id = currentID;
	idMap[state->id] = state;
	currentID++;

	for(auto &capture: state->captures) {
		if(!capture->next->tempMark)
			utilRelabelStates(capture->next);
	}
	for(auto &filter: state->filters) {
		if(!filter->next->tempMark)
			utilRelabelStates(filter->next);
	}
}


std::string ExtendedVA :: pprint() {
  /* Gives a codification for the LogicalVA that can be used to visualize it
     at https://puc-iic2223.github.io . Basically it uses Breath-First Search
     to get every labeled transition in the ExtendedVA with the unique ids for
     each State */


  // Declarations
  std::stringstream ss, cond;
  State *current;
  int cid, nid;  // cid: current State id; nid : next State id
  std::bitset<32> S;

	std::vector<int> capture_states, pcapture_states;

  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<State*> queue;

  // Start on the init State
  visited.insert(init_state_->id);
  queue.push_back(init_state_);

  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();

		if(current->flags() & State::kCaptureState)
			capture_states.push_back(current->id);
		if(current->flags() & State::kPreCaptureState)
			pcapture_states.push_back(current->id);
    cid = current->id;

    // For every capture transition
    for (auto &capture: current->captures) {
      S = capture->code;

      nid = capture->next->id;

      ss << "t " << cid << " " << variable_factory_->print_varset(S) << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture->next);
      }
    }

    // For every filter transition
    for (auto &filter: current->filters) {
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

  // Code initial State
  ss << "i " << init_state_->id;

	// for(int id: capture_states)
	// 	ss << "\nc " << id;

	// for(int id: pcapture_states)
	// 	ss << "\npc " << id;

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

bool ExtendedVA :: utilSearchSuperFinals(State *s) {
	s->colorMark = 'g'; // Mark as grey

	for(auto &filter: s->filters) {
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

std::set<State*> ExtendedVA :: getSubset(BitsetWrapper bs) const {
	std::set<State*> ret;
	for(size_t i=0; i < bs.size(); i++)
		if(bs.get(i))
			ret.insert(idMap.at(i));
	return ret;
}

// Computes the inplace cross product of an ExtendedVA with the two-state
// automaton defined by:
//
//    state | filter   capture
//   --------------------------
//      0   |   1         0
//      1   |   0         1
//
// This is an optimization for the evaluation algorithm. It permits to
// leave out the need to check correct NodeList transpassing between
// transitioning DetStates at reading() step.
void ExtendedVA::crossProdOpt() {

	// --- Duplicate states --- //

	std::vector<State*> states0, states1;

	// State pointer to index at statevector
	std::map<State*, size_t> ptr2index;
	size_t initStateIdx;

	for (size_t i = 0; i < states.size(); ++i) {
		states0.push_back(new State());
		states1.push_back(new State());

		// Store state's ptr corresponding index
		ptr2index[states[i]] = i;

		if(states[i] == initState())
			initStateIdx = i;

		// Set final states
		if(states[i]->isFinal) {
			states0.back()->setFinal(true);
			states1.back()->setFinal(true);
		}
	};

	// --- Connect new states --- //

	// Here p and q represent transition p->q
	State *pOld, *qOld, *p0New, *p1New, *q0New, *q1New;
	size_t qOldIdx; // Needed for pairing new states to old states
	for (size_t i = 0; i < states.size(); ++i) {
		pOld = states[i];
		p0New = states0[i]; p1New = states1[i];
		p0New->flags_ = pOld->flags_;
		p1New->flags_ = pOld->flags_;
		for (auto const &filter: pOld->filters) {
			qOld = filter->next;
			qOldIdx = ptr2index[qOld];

			q0New = states0[qOldIdx]; q1New = states1[qOldIdx];

			p0New->addFilter(filter->code, q1New);
			p1New->addFilter(filter->code, q0New);
		}
		for (auto const &capture: pOld->captures) {
			qOld = capture->next;
			qOldIdx = ptr2index[qOld];

			q0New = states0[qOldIdx]; q1New = states1[qOldIdx];

			p0New->addCapture(capture->code, q0New);
			p1New->addCapture(capture->code, q1New);
		}

	}

	// --- Set new states and prune unreachable states --- //

	// Concat both vectors
	states0.insert(states0.end(), states1.begin(), states1.end());

	set_initState(states0[initStateIdx]); // Init state is (q0, 0)
	states = std::move(states0);
	pruneUselessStates();
}

void ExtendedVA::compute_if_dfa_searchable() {
	if(!is_raw_ && variable_factory_->size() == 1) {
		auto ns = init_state_->nextFilter(filter_factory_->getCode('\0'));
		if(ns != nullptr) {
			try {
				auto dot_code = filter_factory_->getCode(ast::special(SpecialCode::kAnyChar, true));
				auto ls = ns->nextFilter(dot_code);
				if(ls != ns) return;

				is_dfa_searchable_ = true;
				return;

				auto var = variable_factory_->variables().front();

				auto nns = ns->nextCapture(variable_factory_->open_code(var));

				if(nns == nullptr) return;

				is_dfa_searchable_ = true;

			} catch (std::out_of_range &e) {
				return;
			}
		}
	}
}

} // end namespace rematch