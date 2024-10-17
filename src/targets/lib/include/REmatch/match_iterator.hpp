#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "constants.hpp"
#include "flags.hpp"
#include "match.hpp"

namespace REmatch {
class Document;
class Mediator;
struct QueryData;
struct Statistics;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {
class MatchIterator {

 private:
  std::unique_ptr<QueryData> query_data_;
  std::unique_ptr<Mediator> mediator_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;

 public:
  MatchIterator(
      QueryData& query_data, const std::string& str,
      uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
      uint_fast32_t max_deterministic_states =
          DEFAULT_MAX_DETERMINISTIC_STATES);
  MatchIterator(
      const std::string& pattern, const std::string& str, Flags flags,
      uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
      uint_fast32_t max_deterministic_states =
          DEFAULT_MAX_DETERMINISTIC_STATES);

  ~MatchIterator();

  std::unique_ptr<Match> next();

  std::unique_ptr<Statistics> stats;
};

}  // end namespace library_interface
}  // namespace REmatch
