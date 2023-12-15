#include "regex.hpp"
#include "exceptions/dfa_state_limit_checker.hpp"

namespace REMatch {
inline namespace library_interface {

Regex::Regex(std::string_view pattern, Flags flags)
    : flags_(flags), mediation_subjects_([&]() {
        rematch::Parser parser = rematch::Parser(pattern);
        rematch::LogicalVA logical_va = parser.get_logical_va();
        std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog =
            parser.get_variable_catalog();
        rematch::ExtendedVA extended_va = rematch::ExtendedVA(logical_va);
        extended_va.clean_for_determinization();

        auto dfa_states_checker = rematch::DFAStateLimitChecker(flags);
        auto extended_det_va = rematch::ExtendedDetVA(extended_va, dfa_states_checker);

        return rematch::MediationSubjects{logical_va, extended_det_va,
                                          variable_catalog};
      }()) {
  segment_manager_creator_ =
      rematch::SegmentManagerCreator(mediation_subjects_.logical_va, flags);
}

std::unique_ptr<Match> Regex::find(std::string_view text, Flags flags) {
  MatchIterator iterator = finditer(text, flags);
  return iterator.next();
}

MatchIterator Regex::finditer(std::string_view document_view, Flags flags) {
  ZoneScoped;

  std::string document;

  // add the start and end chars that match anchors
  document.reserve(document_view.size() + 2);
  document.push_back(rematch::START_CHAR);
  document.append(document_view);
  document.push_back(rematch::END_CHAR);

  auto mediator = rematch::Mediator(mediation_subjects_, segment_manager_creator_, document, flags_);
  MatchIterator iterator = {std::move(mediator), mediation_subjects_.variable_catalog, document_view};
  return iterator;
}

} // end namespace library_interface
} // end namespace rematch
