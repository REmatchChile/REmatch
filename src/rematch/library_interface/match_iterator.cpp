#include "library_interface/match_iterator.hpp"

namespace REMatch {
inline namespace library_interface {
MatchIterator::MatchIterator(rematch::MediationSubjects& mediation_subjects,
                             std::string&& document, Flags flags)
    : mediator_(mediation_subjects, std::move(document), flags),
      variable_catalog_(mediation_subjects.variable_catalog) {}

std::unique_ptr<Match> MatchIterator::next() {
  rematch::mediator::Mapping* mapping = mediator_.next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, variable_catalog_, mediator_);
  }

  rematch::StatsCollector stats_collector;
  stats = stats_collector.collect(&mediator_);

  return nullptr;
}

std::vector<std::string> MatchIterator::variables() {
  return variable_catalog_->variables();
}

}  // end namespace library_interface
} // end namespace rematch
