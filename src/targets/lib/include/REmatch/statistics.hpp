#pragma once

#include <cstddef>

namespace REmatch {

struct Statistics {
  size_t extended_va_size = 0;
  size_t extended_det_va_size = 0;
  size_t search_nfa_size = 0;
  size_t search_dfa_size = 0;
  size_t nodes_allocated = 0;
  size_t nodes_used = 0;
  size_t nodes_reused = 0;
};
}  // namespace REmatch