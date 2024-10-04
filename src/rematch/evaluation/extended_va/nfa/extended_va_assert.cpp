
#include "extended_va_assert.hpp"

namespace REmatch {

bool ExtendedVAAssert::initial_state_has_only_outgoing_transitions(LogicalVA* const& logical_va) {
  LogicalVAState* initial_state = logical_va->initial_state();

  return initial_state->backward_captures_.empty() && initial_state->backward_filters_.empty();
}

bool ExtendedVAAssert::accepting_state_has_only_incoming_transitions(LogicalVA* const& logical_va) {
  LogicalVAState* accepting_state = logical_va->accepting_state();

  return accepting_state->captures.empty() && accepting_state->filters.empty();
}
}  // namespace
