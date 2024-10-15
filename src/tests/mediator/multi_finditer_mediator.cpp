#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"
#include "mediator/mediator/multi_finditer_mediator.hpp"
#include "output_enumeration/extended_mapping.hpp"

namespace REmatch::testing {

TEST_CASE(
    "the multi mediator returns null pointer when there are no mappings") {
  Parser parser = Parser("!x{a}", true);
  auto document = std::make_shared<Document>("b");

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(
      logical_va, Flags::NONE, REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  auto mediator = MultiFinditerMediator(regex_data, document);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();
  REQUIRE(mapping == nullptr);
}

TEST_CASE(
    "the multi mediator returns an empty mapping if there are no captures") {
  Parser parser = Parser("a", true);
  auto document = std::make_shared<Document>("a");

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(
      logical_va, Flags::NONE, REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  auto mediator = MultiFinditerMediator(regex_data, document);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();
  REQUIRE(mapping != nullptr);
}

TEST_CASE(
    "the multi mediator returns the correct mappings for !x{a} over aaa") {
  std::string document = "aaa";
  std::string regex = "!x{a}";

  // the positions are in reverse order, just like how the ECS creates the annotations
  std::vector<ExtendedMapping> expected_mappings = {
      ExtendedMapping({{get_markers("10"), 1}, {get_markers("01"), 0}}),
      ExtendedMapping({{get_markers("10"), 2}, {get_markers("01"), 1}}),
      ExtendedMapping({{get_markers("10"), 3}, {get_markers("01"), 2}}),
  };

  run_multi_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the multi mediator returns the correct mappings for (!x{a})+ over aaa") {
  std::string document = "aaa";
  std::string regex = "^(!x{a})+$";

  std::vector<ExtendedMapping> expected_mappings = {
      ExtendedMapping({
          {get_markers("10"), 3},
          {get_markers("11"), 2},
          {get_markers("11"), 1},
          {get_markers("01"), 0},
      }),
  };

  run_multi_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the multi mediator returns the correct mappings when using nested "
    "variables") {
  std::string document = "aaaa";
  std::string regex = "^!x{!y{a+}}$";

  std::vector<ExtendedMapping> expected_mappings = {
      ExtendedMapping({
          {get_markers("1010"), 4},
          {get_markers("0101"), 0},
      }),
  };
  run_multi_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the multi mediator returns the correct mappings for the example with "
    "words and phrases") {
  std::string document = "The cat slept. The wind blows.";
  std::string regex = "(^|\\. )!x{!y{\\w+}( !y{\\w+})*}(\\.|$)";

  std::vector<ExtendedMapping> expected_mappings = {
      ExtendedMapping({
          {get_markers("1010"), 29},
          {get_markers("0100"), 24},
          {get_markers("1000"), 23},
          {get_markers("0100"), 19},
          {get_markers("1000"), 18},
          {get_markers("0101"), 15},
      }),
      ExtendedMapping({
          {get_markers("1010"), 13},
          {get_markers("0100"), 8},
          {get_markers("1000"), 7},
          {get_markers("0100"), 4},
          {get_markers("1000"), 3},
          {get_markers("0101"), 0},
      }),
  };
  run_multi_mediator_test(regex, document, expected_mappings);
}

}  // namespace REmatch::testing
