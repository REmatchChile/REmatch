#pragma once

#include <memory>
#include <string>
#include <vector>

#include "REmatch_export.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {
class Match;
class MatchGenerator;
enum class Flags : uint8_t;

class REMATCH_EXPORT Query {

 public:
  explicit Query(const std::string& pattern, Flags flags,
                 uint_fast32_t max_mempool_duplications,
                 uint_fast32_t max_deterministic_states);

  Query(Query&& other) noexcept;
  Query& operator=(Query&& other) noexcept;

  ~Query();

  Match findone(const std::string& document) const;

  std::vector<Match> findmany(const std::string& document, uint_fast32_t limit) const;

  std::vector<Match> findall(const std::string& document) const;

  MatchGenerator finditer(const std::string& document) const;

  bool check(const std::string& document);

  std::vector<std::string> variables() const;

 private:
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
};

}  // end namespace library_interface
}  // namespace REmatch
