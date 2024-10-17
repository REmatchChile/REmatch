#include "parsing/logical_variable_set_automaton/logical_va.hpp"

#include <REmatch/exceptions.hpp>

namespace REmatch {
inline namespace parsing {

LogicalVA::LogicalVA()
      {
  init_state_ = new_state();
  accepting_state_ = new_state();
  init_state_->set_initial(true);
  accepting_state_->set_accepting(true);
}

LogicalVA::LogicalVA(CharClass charclass) {
  init_state_ = new_state();
  init_state_->set_initial(true);

  accepting_state_ = new_state();
  accepting_state_->set_accepting(true);

  init_state_->add_filter(charclass, accepting_state_);
}

LogicalVA::LogicalVA(const LogicalVA &A)
    : init_state_(nullptr) {

  std::unordered_map<LogicalVAState*, LogicalVAState*>
    old_state_to_new_state_mapping;

  states.reserve(states.size());

  copy_states(A.states, old_state_to_new_state_mapping);
  copy_transitions(A.states, old_state_to_new_state_mapping);

  init_state_ = old_state_to_new_state_mapping[A.init_state_];
  accepting_state_ = old_state_to_new_state_mapping[A.accepting_state_];
  accepts_epsilon_ = A.accepts_epsilon_;
}

LogicalVA::~LogicalVA() {
  for (auto& state : states) {
    delete state;
  }
}

LogicalVA::LogicalVA(LogicalVA&& other)
    : init_state_(other.init_state_),
      accepting_state_(other.accepting_state_),
      accepts_epsilon_(other.accepts_epsilon_),
      states(std::move(other.states)) {
  other.init_state_ = nullptr;
  other.accepting_state_ = nullptr;
}

LogicalVA& LogicalVA::operator=(LogicalVA&& other) {
  if (this != &other) {
    delete init_state_;
    delete accepting_state_;

    states = std::move(other.states);
    init_state_ = other.init_state_;
    accepting_state_ = other.accepting_state_;

    other.init_state_ = nullptr;
    other.accepting_state_ = nullptr;
  }
  return *this;
}

void LogicalVA::copy_states(
      const std::vector<LogicalVAState*> &old_states,
      std::unordered_map<LogicalVAState*, LogicalVAState*>
        &old_state_to_new_state_mapping) {
  for(auto& q_old: old_states) {
    auto q_new = new LogicalVAState(*q_old);
    old_state_to_new_state_mapping[q_old] = q_new;
    states.push_back(q_new);
  }
}

void LogicalVA::copy_transitions(
      const std::vector<LogicalVAState*> &old_states,
      std::unordered_map<LogicalVAState*, LogicalVAState*>
        &old_state_to_new_state_mapping) {
  for(auto& q_old: old_states) {
    LogicalVAState* q_new = old_state_to_new_state_mapping[q_old];
    for(auto& filt: q_old->filters)
      q_new->add_filter(
          filt->charclass, old_state_to_new_state_mapping[filt->next]);
    for(auto& cap: q_old->captures)
      q_new->add_capture(
          cap->code, old_state_to_new_state_mapping[cap->next]);
    for(auto& eps: q_old->epsilons)
      q_new->add_epsilon(old_state_to_new_state_mapping[eps->next]);
    for(auto& anch: q_old->anchors)
      q_new->add_anchor(anch->is_start(), old_state_to_new_state_mapping[anch->next]);
  }
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

      if (!reached_state->filters.empty() || !reached_state->epsilons.empty() ||
          !reached_state->anchors.empty() || reached_state->accepting()) {
        state->add_epsilon(reached_state);
      }

      for(auto &capture: reached_state->captures) {
        if(!capture->next->tempMark)
          stack.push_back(capture->next);
      }
    }
  }

  for(auto &state: states) {
    for (auto& capture: state->captures) {
      delete capture;
    }
    state->captures.clear();
    state->backward_captures_.clear();
  }
}

void LogicalVA::trim() {
  /**
   * We'll do a simple BFS from the initial and final states (using backwards
   * transitions), storing the states that are reached by both procedures
   */

  for(auto &p: states)
    p->visited_and_useful_marks = 0;

  // Marked constants
  const int kReachable  = 1 << 0;  // 0000 0000 0000 0001
  const int kUseful     = 1 << 1;  // 0000 0000 0000 0010

  std::vector<LogicalVAState*> remaining_states;  // New states vector
  std::deque<LogicalVAState*> queue;

  // Start the search forward from the initial state.
  queue.push_back(init_state_);
  init_state_->visited_and_useful_marks |= kReachable;

  while(!queue.empty()) {
    LogicalVAState* p = queue.front(); queue.pop_front();

    for(auto &f: p->filters) {
      if(!(f->next->visited_and_useful_marks & kReachable)) {
        f->next->visited_and_useful_marks |= kReachable;
        queue.push_back(f->next);
      }
    }

    for(auto &e: p->epsilons) {
      if(!(e->next->visited_and_useful_marks & kReachable)) {
        e->next->visited_and_useful_marks |= kReachable;
        queue.push_back(e->next);
      }
    }

    for(auto &c: p->captures) {
      if(!(c->next->visited_and_useful_marks & kReachable)) {
        c->next->visited_and_useful_marks |= kReachable;
        queue.push_back(c->next);
      }
    }

    for(auto &a: p->anchors) {
      if(!(a->next->visited_and_useful_marks & kReachable)) {
        a->next->visited_and_useful_marks |= kReachable;
        queue.push_back(a->next);
      }
    }
  }

  // Now start the search backwards from the final state.
  queue.push_back(accepting_state_);
  accepting_state_->visited_and_useful_marks |= kUseful;

  while(!queue.empty()) {
    LogicalVAState* p = queue.front(); queue.pop_front();
    for(auto &f: p->backward_filters_) {
      if(!(f->from->visited_and_useful_marks & kUseful)) {
        if(f->from->visited_and_useful_marks & kReachable)
          remaining_states.push_back(f->from);
        f->from->visited_and_useful_marks |= kUseful;
        queue.push_back(f->from);
      }
    }
    for(auto &e: p->backward_epsilons_) {
      if(!(e->from->visited_and_useful_marks & kUseful)) {
        if(e->from->visited_and_useful_marks & kReachable)
          remaining_states.push_back(e->from);
        e->from->visited_and_useful_marks |= kUseful;
        queue.push_back(e->from);
      }
    }
    for(auto &c: p->backward_captures_) {
      if(!(c->from->visited_and_useful_marks & kUseful)) {
        if(c->from->visited_and_useful_marks & kReachable)
          remaining_states.push_back(c->from);
        c->from->visited_and_useful_marks |= kUseful;
        queue.push_back(c->from);
      }
    }
    for(auto &a: p->backward_anchors_) {
      if(!(a->from->visited_and_useful_marks & kUseful)) {
        if(a->from->visited_and_useful_marks & kReachable)
          remaining_states.push_back(a->from);
        a->from->visited_and_useful_marks |= kUseful;
        queue.push_back(a->from);
      }
    }
  }

  // delete useless transitions
  for(auto state: states) {
    state->filters.remove_if(
        [&](LogicalVAFilter* filter) {
        if (filter->next->visited_and_useful_marks != (kReachable | kUseful)) {
          delete filter;
          return true;
        }
        return false;
      });

    state->captures.remove_if(
        [&](LogicalVACapture* capture) {
        if (capture->next->visited_and_useful_marks != (kReachable | kUseful)) {
          delete capture;
          return true;
        }
        return false;
      });

    state->epsilons.remove_if(
        [&](LogicalVAEpsilon* epsilon) {
        if (epsilon->next->visited_and_useful_marks != (kReachable | kUseful)) {
          delete epsilon;
          return true;
        }
        return false;
      });

    state->anchors.remove_if(
        [&](LogicalVAAnchor* anchor) {
        if (anchor->next->visited_and_useful_marks != (kReachable | kUseful)) {
          delete anchor;
          return true;
        }
        return false;
      });
  }

  // Add flags to initial state and accepting state so they are not removed
  if (!(init_state_->visited_and_useful_marks & kUseful)) {
    init_state_->visited_and_useful_marks |= kUseful;
    remaining_states.push_back(init_state_);
  }

  accepting_state_->visited_and_useful_marks |= kReachable;
  remaining_states.push_back(accepting_state_);

  // Delete useless states
  for(auto *p: states) {
    if(p->visited_and_useful_marks != (kReachable | kUseful))
    {
      delete p;
    }
  }

  states.swap(remaining_states);
}

void LogicalVA::cat(LogicalVA &a2) {
  /* Concatenates an LogicalVA a2 to the current LogicalVA (inplace) */

  // Adds eps transitions from final states to a2 init LogicalVAState
  accepting_state_->add_epsilon(a2.init_state_);
  accepting_state_->set_accepting(false);

  a2.init_state_->set_initial(false);

  // Add a2 states to states list
  states.insert(states.end(), a2.states.begin(), a2.states.end());
  a2.states.clear();

  // These lines are not redundant, handle the case when one lva accepts epsilon
  // and the other does not.
  if( has_epsilon() )
    init_state_->add_epsilon(a2.init_state_);
  if( a2.has_epsilon() )
    accepting_state_->add_epsilon(a2.accepting_state_);

  // Set a2 final states as new final states
  accepting_state_ = a2.accepting_state_;

  accepts_epsilon_ = a2.accepts_epsilon_ && accepts_epsilon_;
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
  a2.states.clear();

  accepts_epsilon_ = a2.accepts_epsilon_ || accepts_epsilon_;
}

void LogicalVA::kleene() {
  strict_kleene();
  optional();
}

void LogicalVA::strict_kleene() {
  /* Extends the LogicalVA for strict kleene closure (1 or more times) */
  LogicalVAState* new_accepting_state = new_state();
  accepting_state_->add_epsilon(init_state_);
  accepting_state_->add_epsilon(new_accepting_state);
  set_accepting_state(new_accepting_state);
}

void LogicalVA::optional() {
  /* Extends the LogicalVA for optional closure (0 or 1 time) */
  accepts_epsilon_ = true;
  LogicalVAState* new_initial_state = new_state();
  LogicalVAState* new_final_state = new_state();

  new_initial_state->add_epsilon(init_state_);
  new_initial_state->add_epsilon(new_final_state);

  accepting_state_->add_epsilon(new_final_state);

  set_initial_state(new_initial_state);
  set_accepting_state(new_final_state);
}

void LogicalVA::assign(std::bitset<64> open_code, std::bitset<64> close_code) {
  /* Extends the LogicalVA so it can assign its pattern to a variable */

  if (has_epsilon()) {
    throw REmatch::EmptyWordCaptureException("Empty word capturing is not allowed.");
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
  assert(min >= 0 && "min must be non-negative");
  assert((max == -1 || max >= min) && "max must be greater than min");
  assert(!(max == 0 && min == 0) && "min and max cannot both be zero");

  LogicalVA copied(*this);

  if (min == 0) {
    if (max > 0) {
      // {0, N}
      optional();
      for (int i = 0; i < max - 1; ++i) {
        LogicalVA copied_automaton(copied);
        copied_automaton.optional();
        cat(copied_automaton);
      }
    }
    else {
      // {0, -1}
      kleene();
    }
  }
  else {
    // Strict part
    for (int i = 0; i < min - 1; ++i) {
      LogicalVA copied_automaton(copied);
      cat(copied_automaton);
    }
    // Optional part
    if (max > 0) {
      // {N, M}
      for (int i = min; i < max; ++i) {
        LogicalVA copied_automaton(copied);
        copied_automaton.optional();
        cat(copied_automaton);
      }
    } else {
      // {N, -1}
      LogicalVA copied_automaton(copied);
      copied_automaton.kleene();
      cat(copied_automaton);
    }
  }
}

void LogicalVA::remove_epsilon() {
  for(auto &state: states)
		state->visited_and_useful_marks = -1;

  std::vector<LogicalVAState*> stack;
  stack.reserve(states.size());

	for(auto &root_state: states) {
		root_state->visited_and_useful_marks = root_state->id;

		stack.clear();

		for(auto &epsilon: root_state->epsilons) {
			stack.push_back(epsilon->next);
      epsilon->next->visited_and_useful_marks = root_state->id;
		}

		while(!stack.empty()) {
			LogicalVAState* cstate = stack.back(); stack.pop_back();

      // If we reach the accepting state we'll do nothing, as from there
      // backward epsilon removal is needed.

			for(auto &capture: cstate->captures)
				root_state->add_capture(capture->code, capture->next);

			for(auto &filter: cstate->filters)
				root_state->add_filter(filter->charclass, filter->next);

			for(auto &anchor: cstate->anchors)
        root_state->add_anchor(anchor->is_start(), anchor->next);

			for(auto &epsilon: cstate->epsilons) {
				if(epsilon->next->visited_and_useful_marks != root_state->id) {
          cstate->visited_and_useful_marks = root_state->id;
          stack.push_back(epsilon->next);
        }

			}
		}
	}

  // Backward epsilon removal from accepting state

  stack.clear();

  for(auto &state: states)
		state->visited_and_useful_marks = -1;

  for(auto &epsilon: accepting_state_->backward_epsilons_) {
    stack.push_back(epsilon->from);
    epsilon->from->visited_and_useful_marks = accepting_state_->id;
  }

  while(!stack.empty()) {
    LogicalVAState* cstate = stack.back(); stack.pop_back();

    for(auto &capture: cstate->backward_captures_)
      capture->from->add_capture(capture->code, accepting_state_);

    for(auto &filter: cstate->backward_filters_)
      filter->from->add_filter(filter->charclass, accepting_state_);

    for(auto &anchor: cstate->backward_anchors_)
      anchor->from->add_anchor(anchor->is_start(), accepting_state_);

    for(auto &epsilon: cstate->backward_epsilons_) {
      if(epsilon->from->visited_and_useful_marks != accepting_state_->id) {
        cstate->visited_and_useful_marks = accepting_state_->id;
        stack.push_back(epsilon->from);
      }
		}
  }

  for(auto &state: states) {
    for (auto& epsilon : state->epsilons) {
      delete epsilon;
    }
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

    for(auto &anchor: current->anchors) {
      if(!anchor->next->tempMark) {
        stack.push_back(anchor->next);
        anchor->next->tempMark = true;
      }
    }
  }
}

bool LogicalVA::has_useful_anchors() {
  for (auto &state: states) {
    if (state->anchors.size() > 0)
      return true;
  }

  return false;
}

bool LogicalVA::is_accepting_state_reachable() {
  for (auto &state: states)
    state->visited_and_useful_marks = 0;

  bool reachable = false;

  std::vector<LogicalVAState*> stack;
  stack.push_back(init_state_);

  while (!stack.empty()) {
    LogicalVAState* current = stack.back(); stack.pop_back();
    current->visited_and_useful_marks = 1;

    if (current->accepting()) {
      reachable = true;
      break;
    }

    for (auto &filter: current->filters) {
      if (!filter->next->visited_and_useful_marks)
        stack.push_back(filter->next);
    }

    for (auto &epsilon: current->epsilons) {
      if (!epsilon->next->visited_and_useful_marks)
        stack.push_back(epsilon->next);
    }

    for (auto &capture: current->captures) {
      if (!capture->next->visited_and_useful_marks)
        stack.push_back(capture->next);
    }

    for (auto &anchor: current->anchors) {
      if (!anchor->next->visited_and_useful_marks)
        stack.push_back(anchor->next);
    }
  }

  return reachable;
}

void LogicalVA::set_accepting_state(LogicalVAState* state) {
  accepting_state_->set_accepting(false);
  state->set_accepting(true);
  accepting_state_ = state;
}

void LogicalVA::set_initial_state(LogicalVAState* state) {
  init_state_->set_initial(false);
  state->set_initial(true);
  init_state_ = state;
}

std::ostream& operator<<(std::ostream& os, LogicalVA const& A) {
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

    for (auto &filter: current->filters) {
      nid = filter->next->id;

      os << "t " << cid << " " << filter->charclass << " " << nid << '\n';

      // If not visited enqueue and add to visited
      if (visited.find(nid) == visited.end()) {
        visited.insert(nid);
        queue.push_back(filter->next);
      }
    }

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

    for (auto &anchor: current->anchors) {
      nid = anchor->next->id;
      char symbol = anchor->is_start() ? '^' : '$';
      os << "t " << cid << " " << symbol << " " << nid << '\n';
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

void LogicalVA::add_anchor(bool is_start) {
  init_state_->add_anchor(is_start, accepting_state_);
}

void LogicalVA::remove_useless_anchors() {
  for (auto &state: states)
  {
    state->visited_and_useful_marks = 0;
    for (auto &anchor: state->anchors)
      anchor->useful = false;
  }

  const int visited = 1;

  std::vector<LogicalVAState*> stack;
  LogicalVAState *current_state;

  stack.push_back(init_state_);

  // Mark the states that have useful start anchors
  while (!stack.empty()) {
    current_state = stack.back();
    stack.pop_back();
    current_state->visited_and_useful_marks |= visited;

    for (auto &epsilon: current_state->epsilons) {
      if (!(epsilon->next->visited_and_useful_marks & visited))
        stack.push_back(epsilon->next);
    }

    for (auto &capture: current_state->captures) {
      if (!(capture->next->visited_and_useful_marks & visited))
        stack.push_back(capture->next);
    }

    for (auto &anchor: current_state->anchors) {
      if (anchor->is_start())
        anchor->useful = true;

      if (anchor->is_start() && !(anchor->next->visited_and_useful_marks & visited))
        stack.push_back(anchor->next);
    }
  }

  for (auto &p: states)
    p->visited_and_useful_marks &= ~visited;

  stack.clear();
  stack.push_back(accepting_state_);

  // Mark the states that have useful end anchors
  while (!stack.empty()) {
    current_state = stack.back();
    stack.pop_back();
    current_state->visited_and_useful_marks |= visited;

    for (auto &epsilon: current_state->backward_epsilons_) {
      if (!(epsilon->from->visited_and_useful_marks & visited))
        stack.push_back(epsilon->from);
    }

    for (auto &capture: current_state->backward_captures_) {
      if (!(capture->from->visited_and_useful_marks & visited))
        stack.push_back(capture->from);
    }

    for (auto &anchor: current_state->backward_anchors_) {
      if (!anchor->is_start())
        anchor->useful = true;

      if (!anchor->is_start() && !(anchor->from->visited_and_useful_marks & visited))
        stack.push_back(anchor->from);
    }
  }

  auto anchor_is_not_useful = [](LogicalVAAnchor* anchor) {
    return !anchor->useful;
  };

  // Remove transitions
  std::vector<LogicalVAAnchor*> useless_anchors;
  for (auto& state : states) {
    for (auto& anchor : state->anchors) {
      if (!anchor->useful) {
        useless_anchors.push_back(anchor);
      }
    }
  }

  for (auto& state : states) {
    state->anchors.erase(
      std::remove_if(
        state->anchors.begin(),
        state->anchors.end(),
        anchor_is_not_useful
      ),
      state->anchors.end()
    );

    state->backward_anchors_.erase(
      std::remove_if(
        state->backward_anchors_.begin(),
        state->backward_anchors_.end(),
        anchor_is_not_useful
      ),
      state->backward_anchors_.end()
    );
  }

  for (auto anchor: useless_anchors) {
    delete anchor;
  }
}

}
}
