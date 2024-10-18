#pragma once

#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace REmatch {

class ExtendedVA;

class ExtendedVAAssert {
 public:
  static bool initial_state_has_only_outgoing_transitions(LogicalVA* const& logical_va);
  static bool accepting_state_has_only_incoming_transitions(LogicalVA* const& logical_va);
};
}  // namespace REmatch
