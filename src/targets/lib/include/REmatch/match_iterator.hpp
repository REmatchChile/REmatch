#pragma once

#include <string>

#include "filtering_module/segment_identificator.hpp"
#include "flags.hpp"
#include "match.hpp"
#include "mediator/mediator/mediator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "query_data.hpp"
#include "statistics.hpp"
#include "stats_collector.hpp"

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
                Flags flags = Flags());
  MatchIterator(const std::string& pattern, const std::string& str,
                Flags flags = Flags());

  std::unique_ptr<Match> next();
  std::vector<std::string> variables();

  Statistics stats = {};
};

}  // end namespace library_interface
}  // namespace REmatch
