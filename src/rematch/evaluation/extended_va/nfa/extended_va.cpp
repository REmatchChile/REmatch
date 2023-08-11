#include "extended_va.hpp"

namespace rematch {

ExtendedVA::ExtendedVA(LogicalVA const &logical_va) {
  LogicalVA logical_va_prim(logical_va);
  logical_va_prim.remove_epsilon();
  logical_va_prim.trim();

  assert(ExtendedVAAssert::initial_state_has_only_outgoing_transitions(&logical_va_prim));
  assert(ExtendedVAAssert::accepting_state_has_only_incoming_transitions(&logical_va_prim));

  copy_data_from_logical_va(logical_va_prim);

  capture_closure();

  add_read_captures_transitions();
  add_loop_to_initial_state();
}

void ExtendedVA::copy_data_from_logical_va(LogicalVA &logical_va) {
  std::map<unsigned int, ExtendedVAState*> lva_id_to_eva_state;
  copy_states_from_logical_va(logical_va, lva_id_to_eva_state);
  copy_transitions_from_logical_va(logical_va, lva_id_to_eva_state);
}

void ExtendedVA::copy_states_from_logical_va
(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &lva_id_to_eva_state) {
  for (auto &state : logical_va.states) {
    ExtendedVAState* new_state = create_new_state();

    lva_id_to_eva_state[state->id] = new_state;
  }

  LogicalVAState* lva_initial_state = logical_va.initial_state();
  lva_id_to_eva_state[lva_initial_state->id]->set_initial(true);
  initial_state_ = lva_id_to_eva_state[lva_initial_state->id];

  LogicalVAState* lva_accepting_state = logical_va.accepting_state();
  lva_id_to_eva_state[lva_accepting_state->id]->set_accepting(true);
  accepting_state_ = lva_id_to_eva_state[lva_accepting_state->id];
}

void ExtendedVA::copy_transitions_from_logical_va
(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &lva_id_to_eva_state) {
  ExtendedVAState* eva_state;
  ExtendedVAState* next_eva_state;

  for (auto& state : logical_va.states) {
    eva_state = lva_id_to_eva_state[state->id];

    for (auto& filter: state->filters) {
      next_eva_state = lva_id_to_eva_state[filter->next->id];
      eva_state->add_filter(filter->charclass, next_eva_state);
    }

    for (auto&capture : state->captures) {
      next_eva_state = lva_id_to_eva_state[capture->next->id];
      eva_state->add_capture(capture->code, next_eva_state);
    }
  }
}

void ExtendedVA::capture_closure() {
  std::queue<ExtendedVAState*> topOrder = inv_topological_sort();

  ExtendedVAState* current_state;

  while (!topOrder.empty()) {
    current_state = topOrder.front();
    topOrder.pop();

    std::bitset<64> new_code;

    for (auto &capture1 : current_state->captures) {
      for (auto &capture2 : capture1->next->captures) {
        new_code = (capture1->code | capture2->code);
        current_state->add_capture(new_code, capture2->next);
      }
    }
  }

  remove_useless_capture_states();
  remove_useless_capture_transitions();
}

std::queue<ExtendedVAState*> ExtendedVA::inv_topological_sort() {
  std::queue<ExtendedVAState*> *queue = new std::queue<ExtendedVAState*>();

  for (auto &state : states) {
    state->visited = false;
  }

  for (auto &state : states) {
    if (!state->visited) {
      inv_topological_sort_util(state, queue);
    }
  }

  return *queue;
}

void ExtendedVA::inv_topological_sort_util(ExtendedVAState *state, std::queue<ExtendedVAState *> *queue) {
  state->visited = true;

  if (state->captures.empty())
    return; // Not interested if no captures present

  for (auto &capture : state->captures) {
    if (!capture->next->visited) {
      inv_topological_sort_util(capture->next, queue);
    }
  }

  queue->push(state);
}

void ExtendedVA::remove_useless_capture_states() {
	bool capture_state_is_useless;

	for (auto state = states.begin(); state != states.end();) {
		capture_state_is_useless = check_if_capture_state_is_useless(*state);

		if (capture_state_is_useless) {
      (*state)->delete_transitions();
			state = states.erase(state);

		} else {
			++state;
		}
	}
}

bool check_if_capture_state_is_useless(ExtendedVAState* state) {
  return state->backward_filters.empty() &&
    state->filters.empty() &&
    !state->is_accepting() &&
    !state->is_initial();
}

void ExtendedVA::remove_useless_capture_transitions() {
  for (auto &state : states) {
    if (state->backward_filters.empty() && !state->backward_captures.empty())
      state->captures.clear();

    state->captures.erase(
      std::remove_if(
        state->captures.begin(),
        state->captures.end(),
        [](ExtendedVACapture* capture) {
          return capture->next->filters.empty() && !capture->next->is_accepting();
        }
      ),
      state->captures.end()
    );
  }
}

void ExtendedVA::add_read_captures_transitions() {
  // add read captures transitions
  for (auto& state : states) {
    for (auto& capture : state->captures) {

      for (auto& next_filter : capture->next->filters)
        state->add_read_capture(next_filter->charclass, capture->code, next_filter->next);

      if (capture->next->is_accepting()) {
        auto eof = CharClass(-1);
        state->add_read_capture(eof, capture->code, capture->next);
      }
    }
  }

  remove_capture_transitions();

  // replace filter transitions with read captures transitions
  for (auto& state : states) {
    for (auto& filter : state->filters) {
      std::bitset<64> empty_set;
      state->add_read_capture(filter->charclass, empty_set, filter->next);
    }
  }

  remove_filter_transitions();

  trim();
}

void ExtendedVA::remove_capture_transitions() {
  for (auto& state : states) {
    for (auto capture : state->captures)
      delete capture;
  }

  for (auto&state : states) {
    state->captures.clear();
    state->backward_captures.clear();
  }
}

void ExtendedVA::remove_filter_transitions() {
  for (auto& state : states) {
    for (auto filter : state->filters)
      delete filter;
  }

  for (auto&state : states) {
    state->filters.clear();
    state->backward_filters.clear();
  }
}

void ExtendedVA::trim() {
  std::map<ExtendedVAState*, bool> is_useful;
  std::map<ExtendedVAState*, bool> is_reachable;

  for (auto &state : states) {
    is_reachable[state] = false;
    is_useful[state] = false;
  }

  std::vector<ExtendedVAState*> remaining_states;
  std::deque<ExtendedVAState*> queue;

  // Start the search forward from the initial state.
  queue.push_back(initial_state_);
  is_reachable[initial_state_] = true;

  while (!queue.empty()) {
    ExtendedVAState* current_state = queue.front();
    queue.pop_front();

    for (auto &read_capture : current_state->read_captures) {
      ExtendedVAState* next_state = read_capture->next;

      if (!(is_reachable[next_state])) {
        is_reachable[next_state] = true;
        queue.push_back(next_state);
      }
    }
  }

  // Now start the search backwards from the final state.
  queue.push_back(accepting_state_);
  is_useful[accepting_state_] = true;

  while (!queue.empty()) {
    ExtendedVAState* current_state = queue.front();
    queue.pop_front();

    for (auto &read_capture : current_state->backward_read_captures) {
      ExtendedVAState* previous_state = read_capture->from;

      if (!(is_useful[previous_state])) {
        if (is_reachable[previous_state])
          remaining_states.push_back(previous_state);

        is_useful[previous_state] = true;
        queue.push_back(previous_state);
      }
    }
  }

  remaining_states.push_back(accepting_state_);

  // Delete useless states
  for (auto& state : states) {
    if (!(is_useful[state] && is_reachable[state]))
      delete state;
  }

  states.swap(remaining_states);
}

void ExtendedVA::add_loop_to_initial_state() {
  std::bitset<64> empty_set;
  CharClass asterisk_class = CharClass((char) 0, (char) 254);
  initial_state_->add_read_capture(asterisk_class, empty_set, initial_state_);
}

void ExtendedVA::set_accepting_state(ExtendedVAState* state) {
  accepting_state_->set_accepting(false);
  state->set_accepting(true);
  accepting_state_ = state;
}

void ExtendedVA::set_initial_state(ExtendedVAState* state) {
  initial_state_->set_initial(false);
  state->set_initial(true);
  initial_state_ = state;
}

ExtendedVAState* ExtendedVA::create_new_state() {
  ExtendedVAState* new_state = new ExtendedVAState();
  states.push_back(new_state);

  return new_state;
}

std::string remove_leading_zeros(std::bitset<64> code) {
  std::string code_as_string = code.to_string();
  code_as_string.erase(0, code_as_string.find_first_not_of('0'));
  return code_as_string == "" ? "0" : code_as_string;
}

std::ostream& operator<<(std::ostream& os, ExtendedVA const &extended_va) {
  int current_id, next_id;
  std::bitset<64> capture_set;

  std::unordered_set<unsigned int> visited;

  std::deque<ExtendedVAState*> queue;

  visited.insert(extended_va.initial_state()->id);
  queue.push_back(extended_va.initial_state());

  while(!queue.empty()) {
    ExtendedVAState* current_state = queue.front();
    queue.pop_front();
    current_id = current_state->id;

    for (auto &capture: current_state->captures) {
      capture_set = capture->code;
      next_id = capture->next->id;

      os << "t " << current_id << " " << capture_set << " " << next_id << '\n';

      if (visited.find(next_id) == visited.end()) {
        visited.insert(next_id);
        queue.push_back(capture->next);
      }
    }

    for (auto &filter: current_state->filters) {
      next_id = filter->next->id;

      os << "t " << current_id << " " << filter->charclass << " " << next_id << '\n';

      if (visited.find(next_id) == visited.end()) {
        visited.insert(next_id);
        queue.push_back(filter->next);
      }
    }

    for (auto &read_capture: current_state->read_captures) {
      next_id = read_capture->next->id;

      os << "t " << current_id << " " << read_capture->charclass << "|"
         << remove_leading_zeros(read_capture->captures_set) << " " << next_id
         << '\n';

      if (visited.find(next_id) == visited.end()) {
        visited.insert(next_id);
        queue.push_back(read_capture->next);
      }
    }
  }

  os << "f " << extended_va.accepting_state()->id << '\n';
  os << "i " << extended_va.initial_state()->id;

  return os;
}

}
