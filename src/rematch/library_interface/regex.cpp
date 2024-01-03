#include "regex.hpp"

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

std::unique_ptr<Match> Regex::find(std::string_view text) {
  MatchIterator iterator = finditer(text);
  return iterator.next();
}

MatchIterator Regex::finditer(std::string_view document_view) {
  std::string document = rematch::add_start_and_end_chars(document_view);

  return {mediation_subjects_, segment_manager_creator_, std::move(document), flags_};
}

MatchIterator Regex::finditer(std::ifstream& file_document) {
  std::string document = rematch::read_file_and_add_start_and_end_chars(file_document);

  return {mediation_subjects_, segment_manager_creator_, std::move(document), flags_};
}

} // end namespace library_interface
} // end namespace rematch
