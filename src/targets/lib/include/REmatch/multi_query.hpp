#pragma once

#include <cstdint>
#include "evaluation/document.hpp"
#include "flags.hpp"
#include "multi_match.hpp"
#include "multi_match_iterator.hpp"
#include "query_data.hpp"

namespace REmatch {
inline namespace library_interface {

class MultiQuery {
 private:
  QueryData query_data_;
  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states);

  std::unique_ptr<MultiMatch> findone(const std::string& text);

  std::vector<MultiMatch> findall(const std::string& text);

  std::unique_ptr<MultiMatchIterator> finditer(const std::string& text);

  bool check(const std::string& text);
};
}  // namespace library_interface
}  // namespace REmatch
