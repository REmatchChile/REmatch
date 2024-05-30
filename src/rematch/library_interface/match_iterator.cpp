#include "library_interface/match_iterator.hpp"
#include "mediator/mediator/finditer_mediator.hpp"

#include "evaluation/document.hpp"

using namespace rematch;

namespace REMatch {
inline namespace library_interface {
MatchIterator::MatchIterator(rematch::QueryData& query_data,
                             std::string_view str, Flags flags)
    : variable_catalog_(query_data.variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ = std::make_unique<rematch::FinditerMediator>(query_data, document_, flags);
}

MatchIterator::MatchIterator(const std::string& pattern,
                             std::string_view str,
                             Flags flags)
    : query_data_(rematch::get_query_data(pattern, flags)),
      variable_catalog_(query_data_.value().variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ = std::make_unique<rematch::FinditerMediator>(query_data_.value(),
                                                  document_, flags);
}

std::unique_ptr<Match> MatchIterator::next() {
  rematch::mediator::Mapping* mapping = mediator_->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, variable_catalog_, document_);
  }

  rematch::StatsCollector stats_collector;
  stats = stats_collector.collect(mediator_.get());

  return nullptr;
}

std::vector<std::string> MatchIterator::variables() {
  return variable_catalog_->variables();
}

}  // end namespace library_interface
}  // namespace REMatch
