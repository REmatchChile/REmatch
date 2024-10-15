#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"
#include "mediator/mediator/multi_findone_mediator.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE(
    "the multi findone mediator returns null pointer when there are no "
    "mappings") {
  Parser parser = Parser("!x{a}");
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
  auto mediator = MultiFindoneMediator(regex_data, document);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();
  REQUIRE(mapping == nullptr);
}

TEST_CASE(
    "the multi findone mediator returns an empty mapping if there are no "
    "captures") {
  Parser parser = Parser("a");
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
  auto mediator = MultiFindoneMediator(regex_data, document);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();
  REQUIRE(mapping != nullptr);
}

}  // namespace REmatch::testing
