#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "REmatch/fstream_reader.hpp"
#include "REmatch/multi_match.hpp"
#include "REmatch/multi_match_generator.hpp"
#include "REmatch_export.hpp"

namespace REmatch {
struct QueryData;

class MultiMatchStandard;

inline namespace library_interface {
enum class Flags : uint8_t;

// Represents a REQL query with multi spans support.
class REMATCH_EXPORT MultiQuery {

 public:
  // Creates a multi query from the pattern string.
  explicit MultiQuery(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size);

  MultiQuery(MultiQuery&& other) noexcept;

  MultiQuery& operator=(MultiQuery&& other) noexcept;

  ~MultiQuery();

  // Returns the first match in the document.
  std::optional<MultiMatch> findone(const std::string& document) const;

  // Returns the first match in the document. It takes a reader as document.
  std::optional<MultiMatch> findone(Reader* reader) const;

  // Returns a vector containing up to `limit` results from the document.
  std::vector<MultiMatch> findmany(const std::string& document, uint_fast32_t limit) const;

  // Returns a vector containing up to `limit` results from the document. It takes a reader as document.
  std::vector<MultiMatch> findmany(Reader* reader, uint_fast32_t limit) const;

  // Returns a vector containing all matches found in the document.
  std::vector<MultiMatch> findall(const std::string& document) const;

  // Returns a vector containing all matches found in the document. It takes a reader as document.
  std::vector<MultiMatch> findall(Reader* reader) const;

  // Returns an iterator that produces all the matches in the document.
  MultiMatchGenerator finditer(const std::string& document) const;

  // Returns an iterator that produces all the matches in the document. It takes a reader as document.
  MultiMatchGenerator finditer(Reader* reader) const;

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
  uint_fast32_t buffer_size;
};
}  // namespace library_interface
}  // namespace REmatch
