#include "library_interface/match_iterator.hpp"
#include "mediator/mediator/finditer_mediator.hpp"

namespace REMatch {
inline namespace library_interface {
MatchIterator::MatchIterator(rematch::RegexData& regex_data,
                             std::string&& document, Flags flags)
    : variable_catalog_(regex_data.variable_catalog),
      document_(std::move(document)) {
  mediator_ =
      std::make_unique<rematch::FinditerMediator>(regex_data, document_, flags);
}

MatchIterator::MatchIterator(const std::string& pattern, std::string&& document,
                             Flags flags)
    : regex_data_(rematch::get_regex_data(pattern, flags)),
      variable_catalog_(regex_data_.value().variable_catalog),
      document_(std::move(document)) {
  mediator_ = std::make_unique<rematch::FinditerMediator>(regex_data_.value(),
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
