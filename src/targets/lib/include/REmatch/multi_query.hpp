#pragma once

#include <cstdint>

#include "flags.hpp"
#include "multi_match.hpp"
#include "multi_match_iterator.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {

class MultiQuery {

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states);

  MultiQuery(MultiQuery&& other) noexcept;

  MultiQuery& operator=(MultiQuery&& other) noexcept;

  ~MultiQuery();

  std::unique_ptr<MultiMatch> findone(const std::string& document);

  // TODO: this should be changed to std::vector<MultiMatch>
  std::vector<std::unique_ptr<MultiMatch>> findmany(const std::string& document, uint_fast32_t limit);

  // TODO: this should be changed to std::vector<MultiMatch>
  std::vector<std::unique_ptr<MultiMatch>> findall(const std::string& document);

  MultiMatchIterator finditer(const std::string& document);

  bool check(const std::string& document);

 private:
  std::unique_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
};
}  // namespace library_interface
}  // namespace REmatch
