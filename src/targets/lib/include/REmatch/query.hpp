#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "REmatch/flags.hpp"
#include "REmatch/fstream_reader.hpp"
#include "REmatch/match_generator.hpp"
#include "REmatch/match.hpp"
#include "REmatch_export.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {

class REMATCH_EXPORT Query {

 public:
  explicit Query(const std::string& pattern, Flags flags, uint_fast32_t max_mempool_duplications,
                 uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size);

  Query(Query&& other) noexcept;
  Query& operator=(Query&& other) noexcept;

  Match findone(const std::string& document) const;
  Match findone(Reader* reader) const;

  std::vector<Match> findmany(const std::string& document, uint_fast32_t limit) const;
  std::vector<Match> findmany(Reader* reader, uint_fast32_t limit) const;

  std::vector<Match> findall(const std::string& document) const;
  std::vector<Match> findall(Reader* reader) const;

  MatchGenerator finditer(const std::string& document) const;
  MatchGenerator finditer(Reader* reader) const;

  bool check(const std::string& document);
  bool check(Reader* reader);

  std::vector<std::string> variables() const;

 private:
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
  uint64_t buffer_size;
};

}  // end namespace library_interface
}  // namespace REmatch
