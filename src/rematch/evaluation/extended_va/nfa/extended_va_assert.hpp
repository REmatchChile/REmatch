#ifndef EXTENDED_VA_ASSERT_HPP
#define EXTENDED_VA_ASSERT_HPP

#include "extended_va.hpp"

namespace rematch {

class ExtendedVA;

class ExtendedVAAssert {
 public:
  static bool initial_state_has_only_outgoing_transitions(LogicalVA* const& logical_va);
  static bool accepting_state_has_only_incoming_transitions(LogicalVA* const& logical_va);
};
}  // namespace rematch

#endif
