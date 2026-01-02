#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "REmatch/fstream_reader.hpp"
#include "REmatch/multi_match.hpp"
#include "REmatch/s_multi_match_generator.hpp"
#include "REmatch_export.hpp"

namespace REmatch {
struct QueryData;

inline namespace library_interface {
class MultiMatchStandard;
class MultiMatchGenerator;
enum class Flags : uint8_t;

class REMATCH_EXPORT MultiQuery {

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags,
                      uint_fast32_t max_mempool_duplications,
                      uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size);

  MultiQuery(MultiQuery&& other) noexcept;

  MultiQuery& operator=(MultiQuery&& other) noexcept;

  ~MultiQuery();

  std::unique_ptr<MultiMatch> findone(const std::string& document);
  std::unique_ptr<MultiMatch> findone(Reader* reader);

  std::vector<std::unique_ptr<MultiMatch>> findmany(const std::string& document,
                                                        uint_fast32_t limit);
  std::vector<std::unique_ptr<MultiMatch>> findmany(Reader* reader, uint_fast32_t limit);

  std::vector<std::unique_ptr<MultiMatch>> findall(const std::string& document);
  std::vector<std::unique_ptr<MultiMatch>> findall(Reader* reader);

  MultiMatchGenerator finditer(const std::string& document);
  SMultiMatchGenerator finditer(Reader* reader);

  bool check(const std::string& document);
  bool check(Reader* reader);

  std::vector<std::string> variables() const;

 private:
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications_;
  uint_fast32_t max_deterministic_states_;
  uint_fast32_t buffer_size;
};
}  // namespace library_interface
}  // namespace REmatch
