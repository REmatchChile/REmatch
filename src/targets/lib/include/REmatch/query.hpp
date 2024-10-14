#pragma once

#include <memory>
#include <string>

#include "flags.hpp"
#include "match.hpp"
#include "match_iterator.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {

class Query {

 public:
  explicit Query(const std::string& pattern, Flags flags,
                 uint_fast32_t max_mempool_duplications,
                 uint_fast32_t max_deterministic_states);

  Query(Query&& other) noexcept;
  Query& operator=(Query&& other) noexcept;

  ~Query();

  std::unique_ptr<Match> findone(const std::string& document);

  // TODO: this should be changed to std::vector<Match>
  std::vector<std::unique_ptr<Match>> findmany(const std::string& document, uint_fast32_t limit);

  // TODO: this should be changed to std::vector<Match>
  std::vector<std::unique_ptr<Match>> findall(const std::string& document);

  MatchIterator finditer(const std::string& document);

  bool check(const std::string& document);

 private:
  std::unique_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
};

}  // end namespace library_interface
}  // namespace REmatch
