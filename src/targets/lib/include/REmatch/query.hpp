#pragma once

#include <memory>
#include <string>

#include "flags.hpp"
#include "match.hpp"
#include "match_iterator.hpp"
#include "query_data.hpp"

namespace REmatch {

inline namespace library_interface {

class Query {
 private:
  QueryData query_data_;

  uint_fast32_t max_mempool_duplications_;

 public:
  explicit Query(const std::string& pattern, Flags flags,
                 uint_fast32_t max_mempool_duplications,
                 uint_fast32_t max_deterministic_states);

  std::unique_ptr<Match> findone(const std::string& text);

  std::vector<Match> findall(const std::string& text);

  std::unique_ptr<MatchIterator> finditer(const std::string& text);

  bool check(const std::string& text);
};

}  // end namespace library_interface
}  // namespace REmatch
