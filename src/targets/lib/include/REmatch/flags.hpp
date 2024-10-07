#pragma once

#include <cstddef>

namespace REmatch {
inline namespace library_interface {

struct Flags {
  bool line_by_line = false;
  bool early_output = false;
  size_t max_mempool_duplications = 8;
  size_t max_deterministic_states = 1000;
};

}  // namespace library_interface
}  // namespace REmatch
