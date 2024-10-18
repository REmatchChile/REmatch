#pragma once

#include <cstddef>
#include <REmatch/exceptions.hpp>

namespace REmatch {

class DFAStateLimitChecker {
 public:
  size_t current_amount_of_states = 0;
  size_t max_amount_of_states;

  explicit DFAStateLimitChecker(uint_fast32_t max_amount_of_states)
      : max_amount_of_states(max_amount_of_states) {}

  inline void count_state() {
    current_amount_of_states++;

    if (current_amount_of_states > max_amount_of_states)
      throw REmatch::ComplexQueryException();
  }
};

}  // namespace REmatch
