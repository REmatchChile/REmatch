#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "REmatch/flags.hpp"
#include "REmatch/fstream_reader.hpp"
#include "REmatch/match.hpp"
#include "REmatch/match_generator.hpp"
#include "REmatch_export.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {

// Represents a REQL query.
class REMATCH_EXPORT Query {

 public:
  // Creates a query from the pattern string.
  explicit Query(const std::string& pattern, Flags flags, uint_fast32_t max_mempool_duplications,
                 uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size);

  Query(Query&& other) noexcept;
  Query& operator=(Query&& other) noexcept;

  // Returns the first match in the document.
  Match findone(const std::string& document) const;

  // Returns the first match in the document. It takes a reader as document.
  Match findone(Reader* reader) const;

  // Returns a vector containing up to `limit` results from the document.
  std::vector<Match> findmany(const std::string& document, uint_fast32_t limit) const;

  // Returns a vector containing up to `limit` matches from the document. It takes a reader as document.
  std::vector<Match> findmany(Reader* reader, uint_fast32_t limit) const;

  // Returns a vector containing all matches found in the document.
  std::vector<Match> findall(const std::string& document) const;

  // Returns a vector containing all matches found in the document. It takes a reader as document.
  std::vector<Match> findall(Reader* reader) const;

  // Returns an iterator that produces all the matches in the document.
  MatchGenerator finditer(const std::string& document) const;

  // Returns an iterator that produces all the matches in the document. It takes a reader as document.
  MatchGenerator finditer(Reader* reader) const;

  // Returns true if there is a match in the document and false otherwise.
  bool check(const std::string& document) const;

  // Returns true if there is a match in the document and false otherwise. It takes a reader as document.
  bool check(Reader* reader) const;

  // Returns a vector containing the variables present in the query.
  std::vector<std::string> variables() const;

 private:
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
  uint64_t buffer_size;
};

}  // end namespace library_interface
}  // namespace REmatch
