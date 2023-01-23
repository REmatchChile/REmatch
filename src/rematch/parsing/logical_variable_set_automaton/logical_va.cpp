#include <algorithm>
#include <cassert>
#include <deque>
#include <map>
#include <string>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace rematch {

LogicalVA::LogicalVA()
      {
  init_state_ = new_state();
  accepting_state_ = new_state();
  init_state_->set_initial(true);
  accepting_state_->set_accepting(true);
}

LogicalVA::LogicalVA(unsigned int code) {
  init_state_ = new_state();
  init_state_->set_initial(true);

  accepting_state_ = new_state();
  accepting_state_->set_accepting(true);

  init_state_->add_filter(code, accepting_state_);
}

LogicalVA::LogicalVA(const LogicalVA &A)
    : 
      init_state_(nullptr) {

  // Table to map old states to newly allocated copied states
  std::unordered_map<LogicalVAState*, LogicalVAState*> new_states;

  // Prepare states vectors
  states.reserve(states.size());
  // Iterative Search using stack for cleanliness in function definitions,
  std::vector<std::pair<LogicalVAState*, LogicalVAState*>> stack;

  // First, make a copy of all states
  for(auto& q_old: A.states) {
    LogicalVAState* q_new = new LogicalVAState(*q_old);
    new_states[q_old] = q_new;
    states.push_back(q_new);
  }

  // Then start copying the transitions
  for(auto& q_old: A.states) {
    LogicalVAState* q_new = new_states[q_old];

    for(auto& filt: q_old->filters)
      q_new->add_filter(filt->charclass, new_states[filt->next]);
    for(auto& cap: q_old->captures)
      q_new->add_capture(cap->code, new_states[cap->next]);
    for(auto& eps: q_old->epsilons)
      q_new->add_epsilon(new_states[eps->next]);
  }


  init_state_ = new_states[A.init_state_];
  accepting_state_ = new_states[A.accepting_state_];
  has_epsilon_ = A.has_epsilon_;
}

void LogicalVA::remove_captures() {
  std::vector<LogicalVAState*> stack;
  LogicalVAState *reached_state;

  for(auto &state: states) {
    for(auto &state2: states)
      state2->tempMark = false;

    stack.clear();

    for(auto &capture: state->captures) {
      stack.push_back(capture->next);
      state->tempMark = false;
    }

    while(!stack.empty()) {
      reached_state = stack.back();
      stack.pop_back();

      reached_state->tempMark = true;

      if(!reached_state->filters.empty() || !reached_state->epsilons.empty() || reached_state->accepting())
        state->add_epsilon(reached_state);

      for(auto &capture: reached_state->captures) {
        if(!capture->next->tempMark)
          stack.push_back(capture->next);
      }
    }
  }

  for(auto &state: states) {
    state->captures.clear();
    state->backward_captures_.clear();
  }
}

void LogicalVA::trim() {
  // We'll do a simple BFS from the initial and final states (using backwards
  // transitions), storing the states that are reached by both procedures

  for(auto &p: states)
    p->visitedBy = 0;

  // Marked constants
  const int kReachable  = 1 << 0;  // 0000 0000 0000 0001
  const int kUseful     = 1 << 1;  // 0000 0000 0000 0010

  std::vector<LogicalVAState*> trimmed_states;  // New states vector
  std::deque<LogicalVAState*> queue;

  // Start the search forward from the initial state.
  queue.push_back(init_state_);
  init_state_->visitedBy |= kReachable;

  while(!queue.empty()) {
    LogicalVAState* p = queue.front(); queue.pop_front();

    for(auto &f: p->filters) {
      if(!(f->next->visitedBy & kReachable)) {
        f->next->visitedBy |= kReachable;
        queue.push_back(f->next);
      }
    }

    for(auto &c: p->captures) {
      if(!(c->next->visitedBy & kReachable)) {
        c->next->visitedBy |= kReachable;
        queue.push_back(c->next);
      }
    }
  }

  // Now start the search backwards from the final state.
  queue.push_back(accepting_state_);
  accepting_state_->visitedBy |= kUseful;

  while(!queue.empty()) {
    LogicalVAState* p = queue.front(); queue.pop_front();
    for(auto &f: p->backward_filters_) {
      if(!(f->from->visitedBy & kUseful)) {
        if(f->from->visitedBy & kReachable)
          trimmed_states.push_back(f->from);
        f->from->visitedBy |= kUseful;
        queue.push_back(f->from);
      }
    }
    for(auto &c: p->backward_captures_) {
      if(!(c->from->visitedBy & kUseful)) {
        if(c->from->visitedBy & kReachable)
          trimmed_states.push_back(c->from);
        c->from->visitedBy |= kUseful;
        queue.push_back(c->from);
      }
    }
  }

  trimmed_states.push_back(accepting_state_);

  // Delete useless states
  for(auto *p: states) {
    if(p->visitedBy != (kReachable | kUseful))
      delete p;
  }

  states.swap(trimmed_states);
}

void LogicalVA::cat(LogicalVA &a2) {
  /* Concatenates an LogicalVA a2 to the current LogicalVA (inplace) */

  // Adds eps transitions from final states to a2 init LogicalVAState
  accepting_state_->add_epsilon(a2.init_state_);
  accepting_state_->set_accepting(false);

  a2.init_state_->set_initial(false);

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());

  if( has_epsilon() )
    init_state_->add_epsilon(a2.init_state_);
  if( a2.has_epsilon() )
    accepting_state_->add_epsilon(a2.accepting_state_);

  // Set a2 final states as new final states
  accepting_state_ = a2.accepting_state_;

  has_epsilon_ = a2.has_epsilon_ && has_epsilon_;
}

void LogicalVA::alter(LogicalVA &a2) {
  /* Extends the LogicalVA so it can alternate between itself and an
     LogicalVA a2 */

  // Creates a new init LogicalVAState and connects it to the old init and a2's init
  LogicalVAState* new_init = new_state();
  LogicalVAState* new_accept = new_state();

  new_init->set_initial(true);
  new_accept->set_accepting(true);

  new_init->add_epsilon(init_state_);
  new_init->add_epsilon(a2.init_state_);

  accepting_state_->add_epsilon(new_accept);
  a2.accepting_state_->add_epsilon(new_accept);

  init_state_->set_initial(false);
  a2.init_state_->set_initial(false);

  accepting_state_->set_accepting(false);
  a2.accepting_state_->set_accepting(false);

  init_state_ = new_init;
  accepting_state_ = new_accept;

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());

  has_epsilon_ = a2.has_epsilon_ || has_epsilon_;
}

void LogicalVA::kleene() {
  strict_kleene();
  optional();
}

void LogicalVA::strict_kleene() {
  /* Extends the LogicalVA for strict kleene closure (1 or more times) */


  LogicalVAState* nstate = new_state();

  for(auto& filter:  accepting_state_->backward_filters_)  {
    if(filter->from == init_state_)
      nstate->add_filter(filter->charclass, nstate);
    filter->from->add_filter(filter->charclass, nstate);
  }

  for(auto& capture:  accepting_state_->backward_captures_)  {
    if(capture->from == init_state_)
      nstate->add_capture(capture->code, nstate);
    capture->from->add_capture(capture->code, nstate);
  }

  for(auto& epsilon:  accepting_state_->backward_epsilons_)  {
    epsilon->from->add_epsilon(nstate);
  }

  for(auto& filter:  init_state_->filters)
    nstate->add_filter(filter->charclass, filter->next);

  for(auto& capture:  init_state_->captures)
    nstate->add_capture(capture->code, capture->next);

  for(auto& epsilon:  init_state_->epsilons)
    nstate->add_epsilon(epsilon->next);

}

void LogicalVA :: optional() {
  /* Extends the LogicalVA for optional closure (0 or 1 time) */
  has_epsilon_ = true;
}

void LogicalVA::assign(std::bitset<64> open_code, std::bitset<64> close_code) {
  /* Extends the LogicalVA so it can assign its pattern to a variable */

  if(has_epsilon()) {
    throw BadRegexException("Empty word capturing is not allowed (thrown from LogicalVA).");
  }

  // Create new states
  LogicalVAState* open_state = new_state();
  LogicalVAState* close_state = new_state();

  // Connect new open LogicalVAState with init LogicalVAState
  open_state->add_capture(open_code, init_state_);

  init_state_->set_initial(false);

  init_state_ = open_state;
  init_state_->set_initial(true);

  accepting_state_->add_capture(close_code, close_state);

  accepting_state_->set_accepting(false);

  accepting_state_ = close_state;
  accepting_state_->set_accepting(true);
}

void LogicalVA::repeat(int min, int max) {
  // std::cout << pprint() << "\n\n";
  LogicalVA copied(*this);
  if (min == 0 && max == -1) {
    kleene(); return;
  }

  for(int i=1; i < min-1; i++) {
    LogicalVA copied_automaton(copied);
    cat(copied_automaton);
  }

  if(min > 1) {
    LogicalVA copied_automaton(copied);
    if(max == -1)
      copied_automaton.strict_kleene();
    cat(copied_automaton);
  }

  // This is the optional part
  if(max-min > 0) {
    LogicalVA *copied_automaton, *copied_automaton2;
    if(min == 0) {
      if(max > 1) {
        copied_automaton = new LogicalVA(copied);
        copied_automaton->optional();
        for(int i=min+1; i < max-1; i++) {
          copied_automaton2 = new LogicalVA(copied);
          copied_automaton2->cat(*copied_automaton);
          copied_automaton2->optional();
          copied_automaton = copied_automaton2;
       }
       cat(*copied_automaton);
       optional();
      } else {
        optional();
      }
    } else {
      copied_automaton = new LogicalVA(copied);
      copied_automaton->optional();
      for(int i=min+1; i < max; i++) {
        copied_automaton2 = new LogicalVA(copied);
        copied_automaton2->cat(*copied_automaton);
        copied_automaton2->optional();
        copied_automaton = copied_automaton2;
      }
      cat(*copied_automaton);
    }
  }

  //  std::cout << pprint() << "\n\n";
}

void LogicalVA::remove_epsilon() {
  for(auto &state: states)
		state->visitedBy = -1;

  std::vector<LogicalVAState*> stack;
  stack.reserve(states.size());

	for(auto &root_state: states) {
		root_state->visitedBy = root_state->id;

		stack.clear();

		for(auto &epsilon: root_state->epsilons) {
			stack.push_back(epsilon->next);
      epsilon->next->visitedBy = root_state->id;
		}

		while(!stack.empty()) {
			LogicalVAState* cstate = stack.back(); stack.pop_back();

      // If we reach the accepting state we'll do nothing, as from there
      // backward epsilon removal is needed.

			for(auto &capture: cstate->captures)
				root_state->add_capture(capture->code, capture->next);

			for(auto &filter: cstate->filters)
				root_state->add_filter(filter->charclass, filter->next);

			for(auto &epsilon: cstate->epsilons) {
				if(epsilon->next->visitedBy != root_state->id) {
          cstate->visitedBy = root_state->id;
          stack.push_back(epsilon->next);
        }

			}
		} // end while
	} // end for

  // Backward epsilon removal from accepting state

  stack.clear();

  for(auto &state: states)
		state->visitedBy = -1;

  for(auto &epsilon: accepting_state_->backward_epsilons_) {
    stack.push_back(epsilon->from);
    epsilon->from->visitedBy = accepting_state_->id;
  }

  while(!stack.empty()) {
    LogicalVAState* cstate = stack.back(); stack.pop_back();

    for(auto &capture: cstate->backward_captures_)
      capture->from->add_capture(capture->code, accepting_state_);

    for(auto &filter: cstate->backward_filters_)
      filter->from->add_filter(filter->charclass, accepting_state_);

    for(auto &epsilon: cstate->backward_epsilons_) {
      if(epsilon->from->visitedBy != accepting_state_->id) {
        cstate->visitedBy = accepting_state_->id;
        stack.push_back(epsilon->from);
      }
		}
  }

  for(auto &state: states) {
    state->epsilons.clear();
    state->backward_epsilons_.clear();
  }

}

void LogicalVA::relabel_states() {
  for(auto& state: states) {
    state->tempMark = false;
  }

  std::deque<LogicalVAState*> stack;

  int current_id = 0;

  stack.push_back(init_state_);
  init_state_->tempMark = true;

  while(!stack.empty()) {
    LogicalVAState* current = stack.back(); stack.pop_back();

    current->id = current_id++;

    for(auto &filt: current->filters) {
      if(!filt->next->tempMark) {
        stack.push_back(filt->next);
        filt->next->tempMark = true;
      }
    }

    for(auto &eps: current->epsilons) {
      if(!eps->next->tempMark) {
        stack.push_back(eps->next);
        eps->next->tempMark = true;
      }
    }

    for(auto &cap: current->captures) {
      if(!cap->next->tempMark) {
        stack.push_back(cap->next);
        cap->next->tempMark = true;
      }
    }

  }
}

std::ostream& operator<<(std::ostream& os, LogicalVA const &A) {
  /* Gives a codification for the LogicalVA that can be used to visualize it
     at https://puc-iic2223.github.io . Basically it uses Breath-First Search
     to get every labeled transition in the LogicalVA with the unique ids for
     each LogicalVAState */


  // Declarations
  LogicalVAState *current;
  int cid, nid;  // cid: current LogicalVAState id; nid : next LogicalVAState id
  std::bitset<64> S;

  std::vector<int> capture_states, pcapture_states;

  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::deque<LogicalVAState*> queue;

  // Start on the init LogicalVAState
  visited.insert(A.init_state_->id);
  queue.push_back(A.init_state_);

  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();
    cid = current->id;

    // For every epsilon transition
    for (auto &epsilon: current->epsilons) {
      nid = epsilon->next->id;

      os << "t " << cid << " eps " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(epsilon->next);
      }
    }

    // For every capture transition
    for (auto &capture: current->captures) {
      S = capture->code;

      nid = capture->next->id;

      os << "t " << cid << " " << S << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(capture->next);
      }
    }

    // For every filter transition
    for (auto &filter: current->filters) {
      nid = filter->next->id;

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(filter->next);
      }
    }
  }

  os << "f " << A.accepting_state_->id << '\n';

  // Code initial LogicalVAStatefinal_states
  os << "i " << A.init_state_->id;

  return os;
}

LogicalVAState* LogicalVA::new_state() {
  LogicalVAState *nstate = new LogicalVAState();
  states.push_back(nstate);

  return nstate;
}

} // end namespace rematch
