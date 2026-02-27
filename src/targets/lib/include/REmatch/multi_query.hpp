#pragma once

#include <cstdint>
#include <memory>
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

class REMATCH_EXPORT MultiQuery {

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size);

  MultiQuery(MultiQuery&& other) noexcept;

  MultiQuery& operator=(MultiQuery&& other) noexcept;

  ~MultiQuery();

  MultiMatch findone(const std::string& document) const;
  MultiMatch findone(Reader* reader) const;

  std::vector<MultiMatch> findmany(const std::string& document, uint_fast32_t limit) const;
  std::vector<MultiMatch> findmany(Reader* reader, uint_fast32_t limit) const;

  std::vector<MultiMatch> findall(const std::string& document) const;
  std::vector<MultiMatch> findall(Reader* reader) const;

  MultiMatchGenerator finditer(const std::string& document) const;
  MultiMatchGenerator finditer(Reader* reader) const;

  bool check(const std::string& document) const;
  bool check(Reader* reader) const;

  std::vector<std::string> variables() const;

 private:
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
  uint_fast32_t buffer_size;
};
}  // namespace library_interface
}  // namespace REmatch
