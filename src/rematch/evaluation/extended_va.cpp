#include "extended_va.hpp"

namespace rematch {

ExtendedVA::ExtendedVA(LogicalVA const &A) {
  LogicalVA A_prim(A);
  A_prim.remove_epsilon();
  A_prim.trim();

  copy_data_from_logical_va(A_prim);

  capture_closure();

  // duplicate_opt()
}

void ExtendedVA::copy_data_from_logical_va(LogicalVA A) {
  std::map<unsigned int, ExtendedVAState*> lva_id_to_eva_state;
  copy_states_from_logical_va(A, lva_id_to_eva_state);
  copy_transitions_from_logical_va(A, lva_id_to_eva_state);
}

void ExtendedVA::copy_states_from_logical_va
(LogicalVA A, std::map<unsigned int, ExtendedVAState*> &lva_id_to_eva_state) {
  for (auto &state : A.states) {
    ExtendedVAState* new_state = new ExtendedVAState();
    states.push_back(new_state);

    lva_id_to_eva_state[state->id] = new_state;
  }

  LogicalVAState* lva_initial_state = A.initial_state();
  lva_id_to_eva_state[lva_initial_state->id]->set_initial(true);
  initial_state_ = lva_id_to_eva_state[lva_initial_state->id];

  LogicalVAState* lva_accepting_state = A.accepting_state();
  lva_id_to_eva_state[lva_accepting_state->id]->set_accepting(true);
  accepting_state_ = lva_id_to_eva_state[lva_accepting_state->id];
}

void copy_transitions_from_logical_va
(LogicalVA A, std::map<unsigned int, ExtendedVAState*> &lva_id_to_eva_state) {
  ExtendedVAState* eva_state;
  ExtendedVAState* next_eva_state;

  for (auto& state : A.states) {
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

  ExtendedVAState* cstate;

  while (!topOrder.empty()) {
    cstate = topOrder.front();
    topOrder.pop();

    std::bitset<64> newCode;

    for (auto &capture1 : cstate->captures) {
      for (auto &capture2 : capture1->next->captures) {
        newCode = (capture1->code | capture2->code);
        cstate->add_capture(newCode, capture2->next);
      }
    }
  }

  clean_useless_capture_states();
  clean_useless_capture_transitions();

  set_is_static(check_if_static());
}

std::queue<ExtendedVAState*> ExtendedVA::inv_topological_sort() {
  std::queue<ExtendedVAState*> *Q = new std::queue<ExtendedVAState*>();

  for (auto &state : states) {
    state->tempMark = false;
  }

  for (auto &state : states) {
    if (!state->tempMark) {
      inv_topological_sort_util(state, Q);
    }
  }

  return *Q;
}

void ExtendedVA::inv_topological_sort_util(ExtendedVAState *state, std::queue<ExtendedVAState *> *Q) {
  state->tempMark = true;

  if (state->captures.empty())
    return; // Not interested if no captures present

  for (auto &capture : state->captures) {
    if (!capture->next->tempMark) {
      inv_topological_sort_util(capture->next, Q);
    }
  }

  Q->push(state);
}

void ExtendedVA::clean_useless_capture_states() {
	bool capture_state_is_useless;

	for(auto state = states.begin(); state != states.end(); ) {
		capture_state_is_useless = check_if_capture_state_is_useless(*state);

		if(capture_state_is_useless) {
			// Remove the incident capture transitions on previous states
			for(auto &capture: (*state)->backward_captures) {
				for(auto it=capture->from->captures.begin(); it != capture->from->captures.end(); ) {
					if(capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->from->captures.erase(it);
					else
						++it;
				}
			}

			for(auto &capture: (*state)->captures) {
				for(auto it=capture->next->backward_captures.begin(); it != capture->next->backward_captures.end(); ) {
					if(capture->from == (*it)->from && capture->next == (*it)->next)
						it = capture->next->backward_captures.erase(it);
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

bool check_if_capture_state_is_useless(ExtendedVAState* state) {
  return state->backward_filters.empty() &&
    state->filters.empty() &&
    !state->is_accepting() &&
    !state->is_initial();
}

bool ExtendedVA::check_if_static() {
	bool is_static = true;
	std::bitset<64> currentS = 0;

	for(auto &state: states) {
		if(!state->is_accepting() && !state->backward_captures.empty()) {
			is_static = false;
			break;
		} else if (state->is_accepting()) {
			if(!state->backward_filters.empty() || state->backward_captures.empty()) {
				is_static = false;
				break;
			}

			currentS = state->backward_captures.front()->code;

			for (auto &capture: state->backward_captures) {
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

void ExtendedVA::clean_useless_capture_transitions() {
  
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
