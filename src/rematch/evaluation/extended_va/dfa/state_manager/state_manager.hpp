#pragma once

#include "evaluation/extended_va/dfa/extended_det_va_state.hpp"

namespace REmatch {
class StateManager {
 public:
  virtual ~StateManager() = default;

  virtual void create_initial_state(ExtendedVAState* nfa_initial_state) = 0;
  virtual ExtendedDetVAState* create_state(StatesPtrSet& states_set) = 0;
  virtual ExtendedDetVAState* get_initial_state() = 0;
  virtual ExtendedDetVAState* get_state_from_subset(StatesPtrSet& subset) = 0;
  virtual uint32_t get_num_states() = 0;
  virtual void set_state_initial_phases() = 0;

  virtual void set_phase(int32_t) {};
};
}  // namespace REmatch
