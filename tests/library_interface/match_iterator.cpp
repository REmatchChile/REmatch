#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "mediator/mediator.hpp"
#include "library_interface/match_iterator.hpp"
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"

namespace rematch::testing {
using namespace REMatch::library_interface;

void run_match_iterator_test(std::string regex, std::string document, std::vector<rematch::mediator::Mapping> expected_matches);

TEST_CASE("match iterator returns the correct matches for a simple query") {
  std::string regex = "!x{a}";
  std::string document = "all matches";
  std::vector<rematch::mediator::Mapping> expected_matches = {
      rematch::mediator::Mapping({{"x", {0, 1}}}),
      rematch::mediator::Mapping({{"x", {5, 6}}})};
  run_match_iterator_test(regex, document, expected_matches);
}

TEST_CASE("match iterator returns the correct matches when there is more than one variable") {
  std::string regex = "!a{a+}!b{b+}";
  std::string document = "aabb";

  std::vector<rematch::mediator::Mapping> expected_matches = {
      rematch::mediator::Mapping({{"a", {0, 2}}, {"b", {2, 4}}}),
      rematch::mediator::Mapping({{"a", {0, 2}}, {"b", {2, 3}}}),
      rematch::mediator::Mapping({{"a", {1, 2}}, {"b", {2, 4}}}),
      rematch::mediator::Mapping({{"a", {1, 2}}, {"b", {2, 3}}})};
  run_match_iterator_test(regex, document, expected_matches);
}

void run_match_iterator_test(std::string regex, std::string document, std::vector<rematch::mediator::Mapping> expected_matches) {
  Parser parser = Parser(regex);
  document = START_CHAR + document + END_CHAR;
  std::string_view document_view = document.c_str();

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  auto extended_det_va = ExtendedDetVA(extended_va);
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(logical_va);

  Mediator mediator = Mediator(extended_det_va, variable_catalog, segment_manager_creator, std::move(document));
  auto match_iterator = MatchIterator(std::move(mediator), variable_catalog, document_view);

  std::ostringstream info_os;
  info_os << "Actual mappings:" << std::endl;

  std::unique_ptr<Match> match = match_iterator.next();
  while (match != nullptr) {
    auto actual_match = rematch::mediator::Mapping(match->groupdict());
    info_os << actual_match;
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_matches, actual_match));
    remove_mapping_from_expected(expected_matches, actual_match);
    match = match_iterator.next();
  }
  INFO(info_os.str());
  REQUIRE(expected_matches.size() == 0);
}

}  // namespace rematch::testing
