#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_set>

#include "automata/nfa/state.hpp"
#include "charclass.hpp"
#include "eva.hpp"
#include "factories/factories.hpp"
#include "lva.hpp"
#include "regex/regex_options.hpp"

namespace rematch {

ExtendedVA::ExtendedVA(LogicalVA const &A, Anchor a)
    : variable_factory_(A.varFactory()), filter_factory_(A.filterFactory()),
      anchor_(a) {
  // Copy the VA
  LogicalVA A_prim(A);

  #ifndef NDEBUG
  std::cout << "LogicalVA:\n" << A_prim << "\n\n";
  #endif

  A_prim.remove_epsilon();

  // Swap the VA's state graph for the EvaluationVA
  states.swap(A_prim.states);
  init_state_ = A_prim.initial_state();
  accepting_state_ = A_prim.accepting_state();

  trim();

  // Add searching loop to init state if search is unanchored.
  if (anchor_ == Anchor::kUnanchored) {
    auto code = filter_factory_->add_filter(CharClass(0, CHAR_MAX));
    init_state_->add_filter(code, init_state_);
    // accepting_state_->add_filter(code, accepting_state_);
  }

  #ifndef NDEBUG
  std::cout << "EvaluationVA before offset:\n" << *this << "\n\n";
  #endif

  #ifndef NOPT_OFFSET
  offset_opt();
  #endif

  captureClosure();

  #ifndef NDEBUG
  std::cout << "EvaluationVA after capture closure:\n" << *this << "\n\n";
  #endif

  duplicate_opt();

  relabelStates();

#ifndef NDEBUG
  std::cout << "EvaluationVA after relabeling:\n" << *this << "\n\n";
#endif
}

void ExtendedVA::addCapture(LogicalVA::State *state, std::bitset<32> bs, LogicalVA::State *next) {
  state->add_capture(bs, next);
}

size_t ExtendedVA::size() const { return states.size(); }


bool ExtendedVA::is_ambiguous() const {

  struct StatePair {
    LogicalVA::State* p1 = nullptr;
    LogicalVA::State* p2 = nullptr;
    bool splitted = true;

    bool operator==(const StatePair &rhs) const {
      return (p1->id == rhs.p1->id) &&
             (p2->id == rhs.p2->id) &&
             (splitted == rhs.splitted);
    }
  };

  struct StatePairHasher {
    size_t operator()(const StatePair & sp) const {
      std::string s = std::to_string(sp.p1->id) + " " +
                      std::to_string(sp.p2->id) + " " +
                      std::to_string(sp.splitted);

      return std::hash<std::string>()(s);
    }
  };

  struct IntPairHasher {
    size_t operator()(const std::pair<int, int> &p) const {
      return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
  };

  std::unordered_set<StatePair, StatePairHasher> visited;
  std::queue<StatePair> queue;

  std::unordered_map<std::pair<int, int>, CharClass, IntPairHasher> intersects;

  auto filters = filter_factory_->filters();

  for(size_t i = 0; i < filters.size(); i++) {
    for(size_t j = i; j < filters.size(); j++) {
      CharClass fi = filters[i], fj = filters[j];
      intersects.insert({{i,j},fi.intersect(fj)});
    }
  }

  StatePair first_pair{init_state(), init_state(), false};
  visited.insert(first_pair);
  queue.push(first_pair);

  while(!queue.empty()) {
    auto spair = queue.front(); queue.pop();

    if(spair.splitted && spair.p1->accepting() && spair.p2->accepting())
      return true;

    for(auto &c1: spair.p1->captures) {
      for(auto &c2: spair.p2->captures) {
        if(c1->code == c2->code) {
          StatePair new_pair{c1->next, c2->next, spair.splitted};
          auto rt = visited.insert(new_pair);
          if(rt.second) {
            queue.push(new_pair);
          }
        }
      }
    }

    for(auto &f1: spair.p1->filters) {
      for(auto &f2: spair.p2->filters) {
        if(!intersects[{std::min(f1->code, f2->code), std::max(f1->code, f2->code)}].empty()) {
          bool splitted = spair.splitted || f1->next != f2->next;
          StatePair new_pair{f1->next, f2->next, splitted};
          auto rt = visited.insert(new_pair);
          if(rt.second) { // if insert was succesful
            queue.push(new_pair);
          }
        }
      }
    }
  }

  return false;

}

void ExtendedVA::trim() {
  // We'll do a simple BFS from the initial and final states (using backwards
  // transitions), storing the states that are reached by both procedures

  for (auto &p : states)
    p->visitedBy = 0;

  // Marked constants
  const int kReachable = 1 << 0; // 0000 0000 0000 0001
  const int kUseful = 1 << 1;    // 0000 0000 0000 0010

  std::vector<LogicalVA::State *> trimmed_states; // New states vector
  std::deque<LogicalVA::State *> queue;

  // Start the search forward from the initial state.
  queue.push_back(init_state_);
  init_state_->visitedBy |= kReachable;

  while (!queue.empty()) {
    LogicalVA::State *p = queue.front();
    queue.pop_front();

    for (auto &f : p->filters) {
      if (!(f->next->visitedBy & kReachable)) {
        f->next->visitedBy |= kReachable;
        queue.push_back(f->next);
      }
    }

    for (auto &c : p->captures) {
      if (!(c->next->visitedBy & kReachable)) {
        c->next->visitedBy |= kReachable;
        queue.push_back(c->next);
      }
    }
  }

  // Now start the search backwards from the final state.
  queue.push_back(accepting_state_);
  accepting_state_->visitedBy |= kUseful;

  while (!queue.empty()) {
    LogicalVA::State *p = queue.front();
    queue.pop_front();
    for (auto &f : p->backward_filters_) {
      if (!(f->from->visitedBy & kUseful)) {
        if (f->from->visitedBy & kReachable)
          trimmed_states.push_back(f->from);
        f->from->visitedBy |= kUseful;
        queue.push_back(f->from);
      }
    }
    for (auto &c : p->backward_captures_) {
      if (!(c->from->visitedBy & kUseful)) {
        if (c->from->visitedBy & kReachable)
          trimmed_states.push_back(c->from);
        c->from->visitedBy |= kUseful;
        queue.push_back(c->from);
      }
    }
  }

  trimmed_states.push_back(accepting_state_);

  // Delete useless states
  for (auto *p : states) {
    if (p->visitedBy != (kReachable | kUseful))
      delete p;
  }

  states.swap(trimmed_states);
}

void ExtendedVA::captureClosure() {
  /* Using the modified inverse topological order of the automaton capture
  states computes the capture closure of it */

  // Get the modified topological sort of the e-VA
  std::queue<LogicalVA::State*> topOrder = invTopologicalSort();

  relabelStates();

  LogicalVA::State *itState; // Iterator state

  while (!topOrder.empty()) {
    itState = topOrder.front();
    topOrder.pop();

    std::bitset<32> newCode;

    for (auto &capture1 : itState->captures) {
      for (auto &capture2 : capture1->next->captures) {
        newCode = (capture1->code | capture2->code);
        itState->add_capture(newCode, capture2->next);
      }
    }
  } // while(!topOrder.empty())

  cleanUselessCaptureStates();
  cleanUselessCaptureTransitions();

  set_is_static(check_if_static());
}

void ExtendedVA::cleanUselessCaptureStates() {

	bool isUselessCaptureState;
	// Iterate through every state in the automaton
	for(auto state=states.begin(); state != states.end(); ) {

		isUselessCaptureState = ((*state)->backward_filters_.empty() &&
														(*state)->filters.empty() &&
														!(*state)->accepting() &&
														!(*state)->initial());

		if(isUselessCaptureState) {
			// Remove the incident capture transitions on previous states
			for(auto &capture: (*state)->backward_captures_) {
				for(auto it=capture->from->captures.begin(); it != capture->from->captures.end(); ) {
					if(capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->from->captures.erase(it);
					else
						++it;
				}
			}

			for(auto &capture: (*state)->captures) {
				for(auto it=capture->next->backward_captures_.begin(); it != capture->next->backward_captures_.end(); ) {
					if(capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->next->backward_captures_.erase(it);
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

void ExtendedVA::cleanUselessCaptureTransitions() {
  for (auto &state : states) {
    if (state->backward_filters_.empty() &&
        !state->backward_captures_.empty()) {
      state->captures.clear();
    }
    for (auto capture = state->captures.begin();
         capture != state->captures.end();) {
      if ((*capture)->next->filters.empty() && !(*capture)->next->accepting()) {
        (*capture)->next->backward_captures_.remove(*capture);
        capture = state->captures.erase(capture);
      } else {
        capture++;
      }
    }
  }
}

//---------------------------------------------------------------------//
//---------------------------- OFFSET OPTS ----------------------------//
//---------------------------------------------------------------------//

void ExtendedVA::offset_opt() {
  // Vector of lists of capture transitions with the same code
  std::vector<CaptureList> classifier = classifySingleCaptures();

  // Iterate through captures in inverse topological order
  for (auto &capture : inverse_topological_sort_captures()) {
    // Search the corresponding list in the classifier
    for (size_t i = 0; i < 2 * variable_factory_->size(); i++) {
      if (capture->code[i]) {
        compute_offset(classifier[i], i); // Compute offset in bulk
        break;
      }
    }
  }
}

void ExtendedVA::compute_offset(CaptureList &captureList, int codeIndex) {
  std::shared_ptr<LogicalVA::Filter> filter;
  CapturePtr capture;
  LogicalVA::State *p, *q, *r;

  // states p,q,r will label in the following way:
  // (p) ---[capture]---> (q) -----[filter]----> (r)

  while (offset_possible(captureList)) {
    for (auto it = captureList.begin(); it != captureList.end();) {
      capture = *it;
      p = capture->from;
      q = capture->next;

      for (auto &filter : q->filters) {
        r = filter->next;

        // Create new state q'
        LogicalVA::State *q_prim = new LogicalVA::State();
        states.push_back(q_prim);

        filter->next->backward_filters_.remove(filter);
        filter->next = q_prim;

        // Current picture:
        // (p) --[capture]--> (q) ---[filter]---> (q') (r)

        q_prim->add_capture(capture->code, r);

        // Current picture:
        // (p) --[capture]--> (q) ---[filter]---> (q') --[capture]--> (r)

        // Need to add the new capture to the captureList as maybe it's possible
        // to keep offseting the capture in the next iteration
        captureList.push_front(q_prim->captures.front());

        p->add_filter(filter->code, q_prim);

        // Current picture:
        // (p) --[capture]--> (q) ---[filter]---> (q') --[capture]--> (r)
        //  ᴸ--------------[filter]----------------^

        if (q->backward_filters_.empty())
          r->backward_filters_.remove(filter);
      }

      p->captures.remove(capture);
      q->backward_captures_.remove(capture);
      if (q->backward_captures_.empty())
        q->flags_ &= ~LogicalVA::State::kCaptureState;

      // Current picture:
      // (p)                (q) ---[filter]---> (q') --[capture]--> (r)
      //  ᴸ--------------[filter]----------------^

      // Notice that (q) will be eliminated if it's not reachable in the
      // step after the offsets are computed.

      // p->addEpsilon(q);

      it = captureList.erase(it);
    }

    variable_factory_->get_offset(codeIndex)++;
  }
}

bool ExtendedVA::offset_possible(CaptureList &captureList) {
  for (auto &capture : captureList) {
    if (!offset_possible(capture))
      return false;
    for (auto &capture2 : captureList) {
      if (capture2 == capture)
        continue;
      if (!offset_possible(capture, capture2))
        return false;
    }
  }
  return true;
}

bool ExtendedVA::offset_possible(CapturePtr capture1, CapturePtr capture2) {
  for (auto &filter : capture1->next->filters) {
    if (isReachable(filter->next, capture2->next))
      return false;
  }
  return true;
}

bool ExtendedVA::offset_possible(CapturePtr capture) {
  // We want the following:
  // (p) ---[capture]---> (q) -----[filter]----> (r)
  // So we can then change it to:
  // (p) ---[filter]----> (q) ---[capture-1]---> (r)
  // So we need to check:
  // 	1. Capture has only one variable involved.
  //  2. (q) can't be a final state.
  //	3. (q) has at least 1 filter transition and no capture transitions.

  LogicalVA::State *q;
  q = capture->next;

  if (capture->code.count() != 1)
    return false;
  if (q->accepting())
    return false;
  if (q->filters.size() == 0 || q->captures.size() > 0)
    return false;
  if (q->backward_captures_.size() != 1)
    return false;
  for (auto &filter : q->filters) {
    if (isReachable(filter->next, q))
      return false;
  }

  return true;
}

bool ExtendedVA::isReachable(LogicalVA::State *from, LogicalVA::State *end) {
  for (auto &state : states)
    state->tempMark = false;

  std::vector<LogicalVA::State *> stack;
  for (auto &filter : from->filters)
    stack.push_back(filter->next);

  while (!stack.empty()) {
    LogicalVA::State *currentState = stack.back();
    stack.pop_back();
    if (currentState == end)
      return true;
    if (currentState->tempMark)
      continue;
    currentState->tempMark = true;
    for (auto &filter : currentState->filters)
      stack.push_back(filter->next);
  }

  return false;
}

std::vector<CaptureList> ExtendedVA::classifySingleCaptures() {
  std::vector<CaptureList> classifier;

  classifier.resize(2 * variable_factory_->size());

  for (auto &state : states) {
    for (auto &capture : state->captures) {
      if (capture->code.count() == 1) { // If single variable capture

        // Search for the variable code position and store the capture at
        // that position in the classifier
        for (size_t i = 0; i < 2 * variable_factory_->size(); i++) {
          if (capture->code[i]) {
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
CaptureVector ExtendedVA::inverse_topological_sort_captures() {

  CaptureVector totCaptures;
  // Unset flag for each capture and push capture to container.
  for (auto &state : states) {
    for (auto &capture : state->captures) {
      capture->flag = false;
      totCaptures.push_back(capture);
    }
  }

  CaptureVector L; // Returning container
  CapturePtr currCap;

  if (totCaptures.empty())
    return L;

  do {
    currCap = totCaptures.back();
    totCaptures.pop_back();
    if (!currCap->flag)
      getInvTopSortCapturesUtil(currCap, L);

  } while (!totCaptures.empty());

  return L;
}

void ExtendedVA::getInvTopSortCapturesUtil(CapturePtr &cap, CaptureVector &L) {
  if (cap->flag)
    return;

  cap->flag = true;

  for (auto &reachedCap : reachableCaptures(cap)) {
    getInvTopSortCapturesUtil(reachedCap, L);
  }

  L.push_back(cap);
}

CaptureVector ExtendedVA::reachableCaptures(CapturePtr &cap) {
  for (auto &state : states)
    state->tempMark = false;

  std::vector<LogicalVA::State *> stack;
  stack.push_back(cap->next);

  CaptureVector ret;

  LogicalVA::State *s;

  while (!stack.empty()) {
    s = stack.back();
    stack.pop_back();
    s->tempMark = true;
    for (auto &capture : s->captures) {
      ret.push_back(capture);
    }
    for (auto &filter : s->filters) {
      if (!filter->next->tempMark) {
        stack.push_back(filter->next);
      }
    }
  }

  return ret;
}

std::queue<LogicalVA::State *> ExtendedVA ::invTopologicalSort() {
  std::queue<LogicalVA::State *> *Q = new std::queue<LogicalVA::State *>();

  for (auto &state : states) {
    state->tempMark = false;
  }

  for (auto &state : states) {
    if (!state->tempMark) {
      invTopologicalSortUtil(state, Q);
    }
  }

  return *Q;
}

void ExtendedVA::invTopologicalSortUtil(LogicalVA::State *state, std::queue<LogicalVA::State *> *Q) {
  state->tempMark = true;

  if (state->captures.empty())
    return; // Not interested if no captures present

  for (auto &capture : state->captures) {
    if (!capture->next->tempMark) {
      invTopologicalSortUtil(capture->next, Q);
    }
  }

  Q->push(state);

}


void ExtendedVA::relabelStates() {
	for(auto &state: states) {
		state->tempMark = false;
	}

	idMap.clear();
	int counter = 0;

	std::vector<LogicalVA::State*> stack;
	stack.push_back(init_state_);
	init_state_->tempMark = true;

	while(!stack.empty()) {
		auto cstate = stack.back(); stack.pop_back();


		cstate->id = counter++;
		idMap[cstate->id] = cstate;

		for(auto &capture: cstate->captures) {
			if(!capture->next->tempMark) {
				capture->next->tempMark = true;
				stack.push_back(capture->next);
			}
		}
		for(auto &filter: cstate->filters) {
			if(!filter->next->tempMark) {
				filter->next->tempMark = true;
				stack.push_back(filter->next);
			}
		}
	}
}

std::ostream &operator<<(std::ostream &os, ExtendedVA const &A) {
  /* Gives a codification for the LogicalVA that can be used to visualize it
     at https://puc-iic2223.github.io . Basically it uses Breath-First Search
     to get every labeled transition in the ExtendedVA with the unique ids for
     each State */

  // Declarations
  LogicalVA::State *current;
  int cid, nid; // cid: current State id; nid : next State id
  std::bitset<32> S;

  std::vector<int> capture_states, pcapture_states;

  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<LogicalVA::State *> queue;

  // Start on the init State
  visited.insert(A.init_state_->id);
  queue.push_back(A.init_state_);

  // Start BFS
  while (!queue.empty()) {
    current = queue.front();
    queue.pop_front();

    if (current->flags() & LogicalVA::State::kCaptureState)
      capture_states.push_back(current->id);
    if (current->flags() & LogicalVA::State::kPreCaptureState)
      pcapture_states.push_back(current->id);
    cid = current->id;

    // For every capture transition
    for (auto &capture : current->captures) {
      S = capture->code;

      nid = capture->next->id;

      os << "c " << cid << " " << A.variable_factory_->print_varset(S) << " "
         << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture->next);
      }
    }

    // For every filter transition
    for (auto &filter : current->filters) {
      nid = filter->next->id;
      S = filter->code;

      os << "t " << cid << ' ' << A.filter_factory_->get_filter(filter->code)
         << ' ' << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(filter->next);
      }
    }
  }

  // Code final states
  os << "f " << A.accepting_state_->id << '\n';

  // Code initial State
  os << "i " << A.init_state_->id;

  // for(int id: capture_states)
  // 	ss << "\nc " << id;

  // for(int id: pcapture_states)
  // 	ss << "\npc " << id;

  return os;
}

std::set<LogicalVA::State*> ExtendedVA::get_subset(std::vector<bool> bs) const {
  std::set<LogicalVA::State*> ret;
  for (size_t i = 0; i < bs.size(); i++)
    if (bs[i])
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
void ExtendedVA::duplicate_opt() {

  // --- Duplicate states --- //

  std::vector<LogicalVA::State*> states0, states1;

  // State pointer to index at statevector
  std::map<LogicalVA::State*, size_t> ptr2index;
  size_t initStateIdx, acceptingStateIdx;

  for (size_t i = 0; i < states.size(); ++i) {

    if (states[i]->initial())
      initStateIdx = i;
    else if (states[i]->accepting())
      acceptingStateIdx = i;

    states0.push_back(new LogicalVA::State(*states[i]));
    states1.push_back(new LogicalVA::State(*states[i]));

    // Store state's ptr corresponding index
    ptr2index[states[i]] = i;
  };

  // --- Connect new states --- //

  // Here p and q represent transition p->q
  LogicalVA::State *pOld, *qOld, *p0New, *p1New, *q0New, *q1New;
  size_t qOldIdx; // Needed for pairing new states to old states
  for (size_t i = 0; i < states.size(); ++i) {
    pOld = states[i];
    p0New = states0[i];
    p1New = states1[i];
    for (auto const &filter : pOld->filters) {
      qOld = filter->next;
      qOldIdx = ptr2index[qOld];

      q0New = states0[qOldIdx];
      q1New = states1[qOldIdx];

      p1New->add_filter(filter->code, q0New);

      // If reached state is accepting, then only connect to 0
      if (qOld->accepting())
        p0New->add_filter(filter->code, q0New);
      else
        p0New->add_filter(filter->code, q1New);
    }
    for (auto const &capture : pOld->captures) {
      qOld = capture->next;
      qOldIdx = ptr2index[qOld];

      q0New = states0[qOldIdx];
      q1New = states1[qOldIdx];

      p0New->add_capture(capture->code, q0New);

      // If reached state is accepting, then only connect to 0
      if (qOld->accepting())
        p1New->add_capture(capture->code, q0New);
      else
        p1New->add_capture(capture->code, q1New);
    }
  }

  // --- Set new states and prune unreachable states --- //

  // Concat both vectors
  states0.insert(states0.end(), states1.begin(), states1.end());

  set_initial(states0[initStateIdx]); // Init state is (q0, 0)
  set_accepting(states0[acceptingStateIdx]);

  states.swap(states0);
  trim(); // trim the automaton
          // pruneUselessStates();
}

bool ExtendedVA::check_if_static() {
	bool is_static = true;
	std::bitset<32> currentS = 0;
	for(auto &state: states) {
		if(!state->accepting() && !state->backward_captures_.empty()) {
			is_static = false;
			break;
		} else if (state->accepting()) {
			if(!state->backward_filters_.empty() || state->backward_captures_.empty()) {
				is_static = false;
				break;
			}
			currentS = state->backward_captures_.front()->code;
			for (auto &capture: state->backward_captures_) {
				if(currentS != capture->code) {
					is_static = false;
					break;
				}
			}
		}
	}

	static_S_ = currentS;

	return is_static;
}

} // end namespace rematch