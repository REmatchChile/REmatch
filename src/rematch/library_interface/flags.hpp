#ifndef LIBRARY_INTERFACE__FLAGS_HPP
#define LIBRARY_INTERFACE__FLAGS_HPP

#include <cstdarg>
#include <utility>
#include <stdexcept>

namespace REMatch {
inline namespace library_interface {

struct Flags {
  bool line_by_line = false;
  bool early_output = false;
  size_t max_mempool_duplications = 8;
  size_t max_deterministic_states = 1000;
};

}
}

#endif
