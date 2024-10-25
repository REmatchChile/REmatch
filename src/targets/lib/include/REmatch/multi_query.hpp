#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "REmatch_export.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {
class MultiMatch;
class MultiMatchGenerator;
enum class Flags : uint8_t;

class REMATCH_EXPORT MultiQuery {

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states);

  MultiQuery(MultiQuery&& other) noexcept;

  MultiQuery& operator=(MultiQuery&& other) noexcept;

  ~MultiQuery();

  MultiMatch findone(const std::string& document);

  std::vector<MultiMatch> findmany(const std::string& document,
                                   uint_fast32_t limit);

  std::vector<MultiMatch> findall(const std::string& document);

  MultiMatchGenerator finditer(const std::string& document);

  bool check(const std::string& document);

  std::vector<std::string> variables() const;

 private:
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
};
}  // namespace library_interface
}  // namespace REmatch
