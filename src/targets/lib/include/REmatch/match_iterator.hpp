#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "filtering_module/segment_identificator.hpp"
#include "flags.hpp"
#include "match.hpp"
#include "mediator/mediator/mediator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "query_data.hpp"
#include "statistics.hpp"

namespace REmatch {
class Document;

inline namespace library_interface {
class MatchIterator {

 private:
  std::optional<QueryData> query_data_ = std::nullopt;
  std::unique_ptr<Mediator> mediator_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;

 public:
  MatchIterator(QueryData& query_data, const std::string& str,
                uint_fast32_t max_mempool_duplications,
                uint_fast32_t max_deterministic_states);
  MatchIterator(const std::string& pattern, const std::string& str, Flags flags,
                uint_fast32_t max_mempool_duplications,
                uint_fast32_t max_deterministic_states);

  std::unique_ptr<Match> next();
  std::vector<std::string> variables();

  Statistics stats = {};
};

}  // end namespace library_interface
}  // namespace REmatch
