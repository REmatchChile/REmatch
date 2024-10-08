#ifndef DFA_MAXIMUM_AMOUNT_STATES_CHECKER_HPP
#define DFA_MAXIMUM_AMOUNT_STATES_CHECKER_HPP

#include <cstddef>
#include "exceptions/complex_query_exception.hpp"

namespace REmatch {

class DFAStateLimitChecker {
 public:
  size_t current_amount_of_states = 0;
  size_t max_amount_of_states;

  DFAStateLimitChecker(uint_fast32_t max_amount_of_states) :
      max_amount_of_states(max_amount_of_states) {}

  inline void count_state() {
    current_amount_of_states++;

    if (current_amount_of_states > max_amount_of_states)
      throw REmatch::ComplexQueryException();
  }
};

}  // namespace REmatch

#endif
