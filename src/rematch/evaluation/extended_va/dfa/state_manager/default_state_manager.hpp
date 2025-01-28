#pragma once

#include <array>

#include "REmatch/constants.hpp"
#include "evaluation/extended_va/dfa/aliases.hpp"
#include "evaluation/extended_va/dfa/extended_det_va_state.hpp"
#include "exceptions/dfa_state_limit_checker.hpp"
#include "state_manager.hpp"

namespace REmatch {

class DefaultStateManager : public StateManager {
 public:
  std::vector<ExtendedDetVAState*> states;
  std::unordered_map<StatesBitset, ExtendedDetVAState*> bitset_to_state_map;

  ExtendedDetVAState* initial_state = nullptr;
  DFAStateLimitChecker dfa_states_checker_;

  uint64_t extended_va_size;

  explicit DefaultStateManager(int32_t extended_va_size);
  ~DefaultStateManager() override;

  StatesBitset get_bitset_from_states_set(StatesPtrSet& states_set) const;
  ExtendedDetVAState* get_state_from_subset(StatesPtrSet& states_set) override;

  void create_initial_state(ExtendedVAState* nfa_initial_state) override;
  ExtendedDetVAState* get_initial_state() override { return initial_state; };
  ExtendedDetVAState* create_state(StatesPtrSet& states_set) override;
  void set_state_initial_phases() override;
  uint32_t get_num_states() override { return states.size(); };

  ExtendedDetVAState* create_state(StatesPtrSet& states_set, StatesBitset& states_bitset);

  void set_clock_pointer(uint64_t new_clock_pointer);
  ExtendedDetVAState* get_state_to_replace();
};
}  // namespace REmatch
