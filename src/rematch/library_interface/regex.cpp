#include "library_interface/regex.hpp"
#include "parsing/parser.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "mediator/mediator.hpp"

namespace REMatch {
inline namespace library_interface {

rematch::MediationSubjects get_mediation_subjects(std::string_view pattern, Flags flags);

Regex::Regex(std::string_view pattern, Flags flags)
    : mediation_subjects_(get_mediation_subjects(pattern, flags)) {}

rematch::MediationSubjects get_mediation_subjects(std::string_view pattern, Flags flags) {
  rematch::Parser parser = rematch::Parser(pattern);
  rematch::LogicalVA logical_va = parser.get_logical_va();
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog = parser.get_variable_catalog();
  rematch::ExtendedVA extended_va = rematch::ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  auto search_dfa = rematch::SearchDFA(logical_va);
  auto extended_det_va = rematch::ExtendedDetVA(extended_va);

  return {search_dfa, extended_det_va, variable_catalog};
}

std::unique_ptr<Match> Regex::find(std::string_view text, Flags flags) {
  MatchIterator iterator = finditer(text, flags);
  return iterator.next();
}

MatchIterator Regex::finditer(std::string_view text, Flags flags) {
  std::string text_data(text);
  text_data += (char)-1;
  auto mediator = rematch::Mediator(mediation_subjects_, text_data, flags);
  return {std::move(mediator), mediation_subjects_.variable_catalog, text};
}

} // end namespace library_interface
} // end namespace rematch
