#include "default_state_manager.hpp"

namespace REmatch {

DefaultStateManager::DefaultStateManager(int32_t extended_va_size)
    : dfa_states_checker_(REmatch::DEFAULT_MAX_DETERMINISTIC_STATES),
      extended_va_size(extended_va_size) {}

DefaultStateManager::~DefaultStateManager() {
  for (auto& state : states) {
    delete state;
  }
}

void DefaultStateManager::create_initial_state(ExtendedVAState* nfa_initial_state) {
  StatesPtrSet initial_subset;

  initial_subset.insert(nfa_initial_state);
  initial_state = new ExtendedDetVAState(initial_subset);
  initial_state->set_initial(true);
  states.push_back(initial_state);

  dfa_states_checker_.count_state();
  StatesBitset initial_state_bitset = get_bitset_from_states_set(initial_subset);
  bitset_to_state_map[initial_state_bitset] = initial_state;
}

ExtendedDetVAState* DefaultStateManager::create_state(StatesPtrSet& states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);
  return create_state(states_set, states_bitset);
}

ExtendedDetVAState* DefaultStateManager::create_state(StatesPtrSet& states_set,
                                                      StatesBitset& states_bitset) {
  auto new_state = new ExtendedDetVAState(states_set);
  bitset_to_state_map[states_bitset] = new_state;

  states.push_back(new_state);
  dfa_states_checker_.count_state();
  return new_state;
}

ExtendedDetVAState* DefaultStateManager::get_state_from_subset(StatesPtrSet& states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);

  if (bitset_to_state_map.count(states_bitset))
    return bitset_to_state_map[states_bitset];

  ExtendedDetVAState* new_state = create_state(states_set, states_bitset);
  return new_state;
}

StatesBitset DefaultStateManager::get_bitset_from_states_set(StatesPtrSet& states_set) const {
  StatesBitset states_bitset(extended_va_size);

  for (auto& state : states_set) {
    states_bitset[state->id] = true;
  }

  return states_bitset;
}

void DefaultStateManager::set_state_initial_phases() {
  for (auto& state : states) {
    state->set_phase(-1);
  }
}

}  // namespace REmatch
