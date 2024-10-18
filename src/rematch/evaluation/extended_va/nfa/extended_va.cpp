#include "extended_va.hpp"

#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#endif

namespace REmatch {

ExtendedVA::ExtendedVA(LogicalVA const &logical_va) {
  #ifdef TRACY_ENABLE
  ZoneScoped;
  #endif

  LogicalVA logical_va_prim(logical_va);
  logical_va_prim.remove_epsilon();
  logical_va_prim.trim();

  assert(ExtendedVAAssert::initial_state_has_only_outgoing_transitions(&logical_va_prim));
  assert(ExtendedVAAssert::accepting_state_has_only_incoming_transitions(&logical_va_prim));

  copy_data_from_logical_va(logical_va_prim);

  capture_closure();

  add_read_captures_transitions();
}

ExtendedVA::ExtendedVA(ExtendedVA&& other) noexcept
    : states(std::move(other.states)),
      initial_state_(other.initial_state_),
      accepting_state_(other.accepting_state_) {
  other.initial_state_ = nullptr;
  other.accepting_state_ = nullptr;
}

ExtendedVA::~ExtendedVA() {
  for (auto& state : states) {
    delete state;
  }
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

    for (auto& capture : state->captures) {
      next_eva_state = lva_id_to_eva_state[capture->next->id];
      eva_state->add_capture(capture->code, next_eva_state);
    }

    for (auto& anchor : state->anchors) {
      next_eva_state = lva_id_to_eva_state[anchor->next->id];
      CharClass anchor_char = anchor->is_start() ? CharClass(START_CHAR) : CharClass(END_CHAR) ;
      eva_state->add_filter(anchor_char, next_eva_state);
    }
  }
}

void ExtendedVA::clean_for_determinization() {
  #ifdef TRACY_ENABLE
  ZoneScoped;
  #endif

  add_loop_to_initial_state();
  duplicate();
  relabel_states();
}

void ExtendedVA::capture_closure() {
  std::queue<ExtendedVAState*> topOrder = inv_topological_sort();

  ExtendedVAState* current_state;

  while (!topOrder.empty()) {
    current_state = topOrder.front();
    topOrder.pop();

    std::bitset<64> new_code;

    auto captures_size = current_state->captures.size();

    for (size_t i = 0; i < captures_size; ++i) {
      ExtendedVACapture* capture1 = current_state->captures[i];
      for (auto &capture2 : capture1->next->captures) {
        new_code = (capture1->code | capture2->code);
        current_state->add_capture(new_code, capture2->next);
        captures_size = current_state->captures.size();
      }
    }
  }

  remove_useless_capture_states();
}

std::queue<ExtendedVAState*> ExtendedVA::inv_topological_sort() {
  std::queue<ExtendedVAState*> queue = std::queue<ExtendedVAState*>();

  for (auto &state : states) {
    state->visited = false;
  }

  for (auto &state : states) {
    if (!state->visited) {
      inv_topological_sort_util(state, &queue);
    }
  }

  return queue;
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
      delete *state;
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

void ExtendedVA::add_read_captures_transitions() {
  // add read captures transitions
  for (auto& state : states) {
    for (auto& capture : state->captures) {

      for (auto& next_filter : capture->next->filters)
        state->add_read_capture(next_filter->charclass, capture->code, next_filter->next);

      if (capture->next->is_accepting()) {
        // todo: Check if bounds are correct
        auto asterisk_class = CharClass({'\x80', '\x7F'});
        state->add_read_capture(asterisk_class, capture->code, capture->next);
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

void ExtendedVA::duplicate() {
  std::map<ExtendedVAState*, size_t> state_to_index;

  std::vector<ExtendedVAState*> states0, states1;

  size_t initial_state_idx;
  size_t accepting_state_idx;

  for (size_t i = 0; i < states.size(); i++) {
    if (states[i]->is_initial())
      initial_state_idx = i;
    if (states[i]->is_accepting())
      accepting_state_idx = i;

    states0.push_back(new ExtendedVAState());
    states1.push_back(new ExtendedVAState());

    state_to_index[states[i]] = i;
  }

  ExtendedVAState *current_old, *next_old;
  ExtendedVAState *current_new0, *next_new0, *current_new1, *next_new1;

  for (size_t i = 0; i < states.size(); i++) {
    current_old = states[i];
    current_new0 = states0[i];
    current_new1 = states1[i];

    for (auto& read_capture : current_old->read_captures) {
      next_old = read_capture->next;
      size_t next_old_idx = state_to_index[next_old];

      next_new0 = states0[next_old_idx];
      next_new1 = states1[next_old_idx];

      current_new1->add_read_capture(read_capture->charclass, read_capture->captures_set, next_new0);

      // if reached state is accepting, then only connect to 0
      if (next_old->is_accepting())
        current_new0->add_read_capture(read_capture->charclass, read_capture->captures_set, next_new0);
      else
        current_new0->add_read_capture(read_capture->charclass, read_capture->captures_set, next_new1);
    }
  }

  states0.insert(states0.end(), states1.begin(), states1.end());

  set_initial_state(states0[initial_state_idx]);
  set_accepting_state(states0[accepting_state_idx]);

  for (auto& state : states) {
    delete state;
  }

  states.swap(states0);
  trim();
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

  // add the initial and accepting state so they are not removed, in case
  // the accepting state is unreachable due to anchors
  if (!is_useful[initial_state_]) {
    is_useful[initial_state_] = true;
    remaining_states.push_back(initial_state_);
  }

  if (!is_reachable[accepting_state_]) {
    is_reachable[accepting_state_] = true;
  }
  remaining_states.push_back(accepting_state_);

  // Delete useless states
  for (auto& state : states) {
    if (!(is_useful[state] && is_reachable[state])) {
      state->delete_transitions();
      delete state;
    }
  }

  states.swap(remaining_states);
}

void ExtendedVA::add_loop_to_initial_state() {
  std::bitset<64> empty_set;
  initial_state_->add_read_capture(CharClass({'\x80', '\x7F'}), empty_set, initial_state_);
}

#if 0
void ExtendedVA::add_loop_to_initial_state() {
  std::bitset<64> empty_set;
  ExtendedVAState* new_state0;
  ExtendedVAState* new_state1;
  ExtendedVAState* new_state2;

  // 1 byte
  initial_state_->add_read_capture(CharClass({'\x00', '\x7F'}), empty_set, initial_state_);

  // 2 bytes
  new_state0 = create_new_state();
  initial_state_->add_read_capture(CharClass({'\xC2', '\xDF'}), empty_set, new_state0);
  new_state0->add_read_capture(CharClass({'\x80', '\xBF'}), empty_set, initial_state_);

  // 3 bytes
  new_state0 = create_new_state();
  new_state1 = create_new_state();
  initial_state_->add_read_capture(CharClass({'\xE0', '\xEF'}), empty_set, new_state0);
  new_state0->add_read_capture(CharClass({'\x80', '\xBF'}), empty_set, new_state1);
  new_state1->add_read_capture(CharClass({'\x80', '\xBF'}), empty_set, initial_state_);

  // 4 bytes
  new_state0 = create_new_state();
  new_state1 = create_new_state();
  new_state2 = create_new_state();
  initial_state_->add_read_capture(CharClass({'\xF0', '\xF7'}), empty_set, new_state0);
  new_state0->add_read_capture(CharClass({'\x80', '\xBF'}), empty_set, new_state1);
  new_state1->add_read_capture(CharClass({'\x80', '\xBF'}), empty_set, new_state2);
  new_state2->add_read_capture(CharClass({'\x80', '\xBF'}), empty_set, initial_state_);
}
#endif

void ExtendedVA::relabel_states() {

  std::deque<ExtendedVAState*> queue;
  std::map<ExtendedVAState*, bool> visited;

  int current_id = 0;

  queue.push_back(initial_state_);
  visited[initial_state_] = true;

  while (!queue.empty()) {
    ExtendedVAState* current_state = queue.back(); queue.pop_back();

    current_state->id = current_id++;

    for (auto &read_capture : current_state->read_captures) {
      if (!visited[read_capture->next]) {
        queue.push_back(read_capture->next);
        visited[read_capture->next] = true;
      }
    }

    for (auto &filter : current_state->filters) {
      if (!visited[filter->next]) {
        queue.push_back(filter->next);
        visited[filter->next] = true;
      }
    }

    for (auto &capture: current_state->captures) {
      if (!visited[capture->next]) {
        queue.push_back(capture->next);
        visited[capture->next] = true;
      }
    }
  }
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
