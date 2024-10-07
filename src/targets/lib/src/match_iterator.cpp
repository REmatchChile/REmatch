#include <REmatch/match_iterator.hpp>

#include "evaluation/document.hpp"
#include "mediator/mediator/finditer_mediator.hpp"

namespace REmatch {
inline namespace library_interface {
MatchIterator::MatchIterator(QueryData& query_data, const std::string& str,
                             Flags flags)
    : variable_catalog_(query_data.variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ = std::make_unique<FinditerMediator>(query_data, document_, flags);
}

MatchIterator::MatchIterator(const std::string& pattern, const std::string& str,
                             Flags flags)
    : query_data_(get_query_data(pattern, flags)),
      variable_catalog_(query_data_.value().variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ =
      std::make_unique<FinditerMediator>(query_data_.value(), document_, flags);
}

std::unique_ptr<Match> MatchIterator::next() {
  mediator::Mapping* mapping = mediator_->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, variable_catalog_, document_);
  }

  StatsCollector stats_collector;
  stats = stats_collector.collect(mediator_.get());

  return nullptr;
}

std::vector<std::string> MatchIterator::variables() {
  return variable_catalog_->variables();
}

}  // end namespace library_interface
}  // namespace REmatch
