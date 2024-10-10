#include <REmatch/match_iterator.hpp>

#include <REmatch/stats_collector.hpp>

#include "evaluation/document.hpp"
#include "mediator/mediator/finditer_mediator.hpp"

namespace REmatch {
inline namespace library_interface {
MatchIterator::MatchIterator(QueryData& query_data, const std::string& str,
                             uint_fast32_t max_mempool_duplications, uint_fast32_t max_deterministic_states)
    : variable_catalog_(query_data.variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ = std::make_unique<FinditerMediator>(query_data, document_,
                                                 max_mempool_duplications, max_deterministic_states);
}

MatchIterator::MatchIterator(const std::string& pattern, const std::string& str,
                             Flags flags,
                             uint_fast32_t max_mempool_duplications,
                             uint_fast32_t max_deterministic_states)
    : query_data_(get_query_data(pattern, flags, max_deterministic_states)),
      variable_catalog_(query_data_.value().variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ = std::make_unique<FinditerMediator>(query_data_.value(), document_,
                                                 max_mempool_duplications,
                                                 max_deterministic_states);
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
