#include "clock_state_manager.hpp"

namespace REmatch {

ClockStateManager::ClockStateManager(int32_t extended_va_size, uint32_t max_amount_of_states)
    : states(max_amount_of_states),
      extended_va_size(extended_va_size),
      max_amount_of_states(max_amount_of_states) {}

ClockStateManager::~ClockStateManager() {
  for (uint32_t i = 0; i < num_states; ++i) {
    delete states[i];
  }
}

void ClockStateManager::create_initial_state(ExtendedVAState* nfa_initial_state) {
  StatesPtrSet initial_subset;

  initial_subset.insert(nfa_initial_state);
  initial_state = new ExtendedDetVAState(initial_subset);
  initial_state->set_initial(true);
  states[num_states++] = initial_state;

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

  if (num_states < max_amount_of_states) {
    return alloc_new_state(states_set, states_bitset);

  } else {
    ExtendedDetVAState* old_state = get_state_to_replace();

    if (old_state == nullptr) {
      return alloc_new_state(states_set, states_bitset);
    }
    StatesPtrSet old_subset = old_state->get_states_subset();
    StatesBitset old_bitset = get_bitset_from_states_set(old_subset);
    bitset_to_state_map.erase(old_bitset);

    old_state->reset(states_set);
    bitset_to_state_map[states_bitset] = old_state;

    new_state = old_state;
  }
  return new_state;
}

ExtendedDetVAState* ClockStateManager::alloc_new_state(StatesPtrSet& states_set,
                                                       StatesBitset& states_bitset) {
  auto new_state = new ExtendedDetVAState(states_set);
  bitset_to_state_map[states_bitset] = new_state;
  states[num_states++] = new_state;
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
  uint32_t initial_clock = clock_pointer;

  while (true) {
    clock_pointer = (clock_pointer + 1) % max_amount_of_states;

    if (initial_clock == clock_pointer) {
      if (second_round) {
        second_round = false;
        duplicate();
        return nullptr;
      } else {
        second_round = true;
      }
    }

    auto* candidate_state = states[clock_pointer];

    if (candidate_state->phase == phase + 1 || candidate_state->phase == phase ||
        candidate_state->phase == phase - 1 || candidate_state->phase == -1 ||
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

void ClockStateManager::duplicate() {
  max_amount_of_states *= 2;
  states.resize(max_amount_of_states);
}

void ClockStateManager::set_state_initial_phases() {
  for (size_t i = 0; i < num_states; ++i) {
    states[i]->set_phase(-1);
  }
}

void ClockStateManager::set_phase(int32_t new_phase) {
  phase = new_phase;
}

}  // namespace REmatch
