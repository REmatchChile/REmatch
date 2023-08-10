#include "extended_va.hpp"

namespace rematch {

ExtendedVA::ExtendedVA(LogicalVA const &logical_va) {
  LogicalVA logical_va_prim(logical_va);
  logical_va_prim.remove_epsilon();
  logical_va_prim.trim();

  copy_data_from_logical_va(logical_va_prim);

  capture_closure();

  add_read_captures_transitions();
}

void ExtendedVA::copy_data_from_logical_va(LogicalVA &logical_va) {
  std::map<unsigned int, ExtendedVAState*> lva_id_to_eva_state;
  copy_states_from_logical_va(logical_va, lva_id_to_eva_state);
  copy_transitions_from_logical_va(logical_va, lva_id_to_eva_state);
}

void ExtendedVA::copy_states_from_logical_va
(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &lva_id_to_eva_state) {
  for (auto &state : logical_va.states) {
    ExtendedVAState* new_state = new ExtendedVAState();
    states.push_back(new_state);

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

  clean_useless_capture_states();
  clean_useless_capture_transitions();
}

std::queue<ExtendedVAState*> ExtendedVA::inv_topological_sort() {
  std::queue<ExtendedVAState*> *queue = new std::queue<ExtendedVAState*>();

  for (auto &state : states) {
    state->temp_mark = false;
  }

  for (auto &state : states) {
    if (!state->temp_mark) {
      inv_topological_sort_util(state, queue);
    }
  }

  return *queue;
}

void ExtendedVA::inv_topological_sort_util(ExtendedVAState *state, std::queue<ExtendedVAState *> *queue) {
  state->temp_mark = true;

  if (state->captures.empty())
    return; // Not interested if no captures present

  for (auto &capture : state->captures) {
    if (!capture->next->temp_mark) {
      inv_topological_sort_util(capture->next, queue);
    }
  }

  queue->push(state);
}

void ExtendedVA::clean_useless_capture_states() {
	bool capture_state_is_useless;

	for (auto state = states.begin(); state != states.end();) {
		capture_state_is_useless = check_if_capture_state_is_useless(*state);

		if (capture_state_is_useless) {
			// Remove the incident capture transitions on previous states
			for (auto &capture: (*state)->backward_captures) {
				for (auto it=capture->from->captures.begin(); it != capture->from->captures.end();) {
					if (capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->from->captures.erase(it);
					else
						++it;
				}
			}

			for (auto &capture: (*state)->captures) {
				for (auto it=capture->next->backward_captures.begin(); it != capture->next->backward_captures.end();) {
					if (capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->next->backward_captures.erase(it);
					else
						++it;
				}
			}
			// Remove the state from the list of states
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

void ExtendedVA::clean_useless_capture_transitions() {
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
  std::vector<ExtendedVAState*> useless_states;

  // add read captures transitions and store intermediate states
  for (auto& state : states) {
    for (auto& capture : state->captures) {

      for (auto& next_filter : capture->next->filters) {
        state->add_read_capture(next_filter->charclass, capture->code, next_filter->next);
        useless_states.push_back(capture->next);
      }

      if (capture->next->is_accepting()) {
        auto eof = CharClass(-1);
        state->add_read_capture(eof, capture->code, capture->next);
      }
    }
  }

  delete_states(useless_states);

  // replace filter transitions with read captures transitions
  for (auto& state : states) {
    for (auto& filter : state->filters) {
      std::bitset<64> empty_set;
      state->add_read_capture(filter->charclass, empty_set, filter->next);
    }
  }

  remove_filters_and_captures();
}

void ExtendedVA::delete_states(std::vector<ExtendedVAState*> states_to_delete) {
  for (auto& state : states_to_delete) {
    state->filters.clear();
    state->backward_filters.clear();
    state->captures.clear();
    state->backward_captures.clear();    
  }

  states.erase(
    std::remove_if(
      states.begin(),
      states.end(),
      [&](ExtendedVAState* state) {
        return std::find(
          states_to_delete.begin(), states_to_delete.end(), state
        ) != states_to_delete.end();
      }
    ),
    states.end()
  );
}

void ExtendedVA::remove_filters_and_captures() {
  for (auto& state : states) {
    for (auto filter : state->filters)
      delete filter;

    for (auto capture : state->captures)
      delete capture;
  }

  for (auto&state : states) {
    state->filters.clear();
    state->backward_filters.clear();
    state->captures.clear();
    state->backward_captures.clear();
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

}
