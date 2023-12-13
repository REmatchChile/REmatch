#include "library_interface/match_iterator.hpp"

namespace REMatch {
inline namespace library_interface {
MatchIterator::MatchIterator(
    rematch::Mediator&& mediator,
    std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
    std::string_view& document)
    : mediator_(std::move(mediator)),
      variable_catalog_(variable_catalog),
      document_(document) {}

std::unique_ptr<Match> MatchIterator::next() {
  rematch::mediator::Mapping* mapping = mediator_.next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, variable_catalog_, document_);
  }

  rematch::StatsCollector stats_collector;
  stats = stats_collector.collect(&mediator_);

  return nullptr;
}

} // end namespace library_interface
} // end namespace rematch
