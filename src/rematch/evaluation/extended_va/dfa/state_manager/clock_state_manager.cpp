#include "clock_state_manager.hpp"

namespace REmatch {

  // TODO: it should use a parameter here
ClockStateManager::ClockStateManager(int32_t extended_va_size)
    : dfa_states_checker_(REmatch::DEFAULT_MAX_DETERMINISTIC_STATES),
      extended_va_size(extended_va_size) {}

ClockStateManager::~ClockStateManager() {
  // TODO:
  for (auto& state : states) {}
}

void ClockStateManager::create_initial_state(ExtendedVAState* nfa_initial_state) {
  StatesPtrSet initial_subset;

  initial_subset.insert(nfa_initial_state);
  initial_state = new ExtendedDetVAState(initial_subset);
  initial_state->set_initial(true);

  dfa_states_checker_.count_state();
  StatesBitset initial_state_bitset = get_bitset_from_states_set(initial_subset);
  bitset_to_state_map[initial_state_bitset] = initial_state;
}

ExtendedDetVAState* ClockStateManager::create_state(StatesPtrSet& states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);
  return create_state(states_set, states_bitset);
}

ExtendedDetVAState* ClockStateManager::create_state(StatesPtrSet& states_set,
                                                    StatesBitset& states_bitset) {
  ExtendedDetVAState* new_state;

  if (num_states < REmatch::DEFAULT_MAX_DETERMINISTIC_STATES) {
    new_state = new ExtendedDetVAState(states_set);
    bitset_to_state_map[states_bitset] = new_state;
    states[num_states++] = new_state;
    dfa_states_checker_.count_state();

  } else {
    ExtendedDetVAState* old_state = get_state_to_replace();

    std::vector<ExtendedVAState*> old_subset_vector = old_state->get_states_subset();
    StatesPtrSet old_subset_set(old_subset_vector.begin(), old_subset_vector.end());
    StatesBitset old_bitset = get_bitset_from_states_set(old_subset_set);
    bitset_to_state_map.erase(old_bitset);

    old_state->reset(states_set);
    bitset_to_state_map[states_bitset] = old_state;

    new_state = old_state;
  }
  return new_state;
}

ExtendedDetVAState* ClockStateManager::get_state_from_subset(StatesPtrSet& states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);

  if (bitset_to_state_map.count(states_bitset))
    return bitset_to_state_map[states_bitset];

  ExtendedDetVAState* new_state = create_state(states_set, states_bitset);
  return new_state;
}

StatesBitset ClockStateManager::get_bitset_from_states_set(StatesPtrSet& states_set) const {
  StatesBitset states_bitset(extended_va_size);

  for (auto& state : states_set) {
    states_bitset[state->id] = true;
  }

  return states_bitset;
}

ExtendedDetVAState* ClockStateManager::get_state_to_replace() {
  // Si da dos vueltas, se duplica el tamaño del buffer states
  while (true) {
    clock_pointer = (clock_pointer + 1) % DEFAULT_MAX_DETERMINISTIC_STATES;

    auto* candidate_state = states[clock_pointer];

    if (candidate_state->phase == phase || candidate_state->phase == phase - 1 ||
        candidate_state->is_initial()) {
      continue;
    }

    if (candidate_state->second_chance) {
      candidate_state->second_chance = false;
      continue;
    }

    return candidate_state;
  }
}

void ClockStateManager::set_state_initial_phases() {
  for (size_t i = 0; i < num_states; ++i) {
    states[i]->set_phase(-1);
  }
}

}  // namespace REmatch
