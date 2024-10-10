#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
  MatchIterator(QueryData& query_data, const std::string& str,
                uint_fast32_t max_mempool_duplications);

  MatchIterator(const std::string& pattern, const std::string& str, Flags flags,
                uint_fast32_t max_deterministic_states,
                uint_fast32_t max_mempool_duplications);

  ~MatchIterator();

  std::unique_ptr<Match> next();
  std::vector<std::string> variables();

  std::unique_ptr<Statistics> stats;
};

}  // end namespace library_interface
}  // namespace REmatch
