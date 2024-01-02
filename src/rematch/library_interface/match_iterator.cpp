#include "library_interface/match_iterator.hpp"

namespace REMatch {
inline namespace library_interface {
MatchIterator::MatchIterator(
    rematch::Mediator&& mediator,
    std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
    std::string_view document)
    : mediator_(std::move(mediator)),
      variable_catalog_(variable_catalog),
      document_(document) {}

MatchIterator::MatchIterator(
    rematch::MediationSubjects& mediation_subjects,
    rematch::SegmentManagerCreator& segment_manager_creator,
    std::string_view document_view, Flags flags)
    : mediator_(mediation_subjects, segment_manager_creator,
                rematch::add_start_and_end_chars(document_view), flags),
      variable_catalog_(mediation_subjects.variable_catalog),
      document_(document_view) {}

MatchIterator::MatchIterator(
    rematch::MediationSubjects& mediation_subjects,
    rematch::SegmentManagerCreator& segment_manager_creator,
    std::string&& document, Flags flags)
    : mediator_(mediation_subjects, segment_manager_creator, std::move(document), flags),
      variable_catalog_(mediation_subjects.variable_catalog),
      document_(rematch::get_document_as_string_view(std::move(document))) {}

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
