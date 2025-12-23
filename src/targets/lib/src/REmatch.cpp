#include <REmatch/REmatch.hpp>

namespace REmatch {
inline namespace library_interface {

Query reql(const std::string& pattern, Flags flags,
           uint_fast32_t max_mempool_duplications,
           uint_fast32_t max_deterministic_states) {
  return Query(pattern, flags, max_mempool_duplications,
               max_deterministic_states);
}

SQuery reql_stream(const std::string& pattern, Flags flags,
                   uint_fast32_t max_mempool_duplications,
                   uint_fast32_t max_deterministic_states,
                   uint_fast32_t buffer_size) {
  return SQuery(pattern, flags, max_mempool_duplications,
                max_deterministic_states, buffer_size);
}

MultiQuery multi_reql(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states) {
  return MultiQuery(pattern, flags, max_mempool_duplications,
                    max_deterministic_states);
}

}  // namespace library_interface
}  // namespace REmatch