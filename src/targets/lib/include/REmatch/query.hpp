#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "REmatch/fstream_reader.hpp"
#include "REmatch/match_generator_type_erased.hpp"
#include "REmatch/match_type_erased.hpp"
#include "REmatch/s_match_generator.hpp"
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

  MatchTypeErased findone(const std::string& document) const;
  MatchTypeErased findone(Reader* reader) const;

  std::vector<MatchTypeErased> findmany(const std::string& document, uint_fast32_t limit) const;
  std::vector<MatchTypeErased> findmany(Reader* reader, uint_fast32_t limit) const;

  std::vector<MatchTypeErased> findall(const std::string& document) const;
  std::vector<MatchTypeErased> findall(Reader* reader) const;

  MatchGeneratorTypeErased finditer(const std::string& document) const;
  MatchGeneratorTypeErased finditer(Reader* reader) const;

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
