#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "mediator/mediator/finditer_mediator.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "library_interface/query.hpp"

namespace rematch::testing {
using namespace REMatch::library_interface;

void run_mediator_test(std::string query, std::string document,
                       std::vector<mediator::Mapping> expected_mappings);

TEST_CASE("the mediator throws an exception when the variable is not in the regex") {
  Parser parser = Parser("!x{a}");
  std::string document_ = "a";
  auto document = std::make_shared<Document>(document_);

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(logical_va);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  auto mediator = FinditerMediator(regex_data, document);

  mediator::Mapping* mapping = mediator.next();
  REQUIRE_THROWS_AS(mapping->get_span_of_variable("y"), VariableNotFoundException);
}

TEST_CASE(
    "the regex object raises an exception when the variable is not in the "
    "regex") {
  auto regex = REMatch::Query("!x{a}");
  std::unique_ptr<Match> match = regex.findone("a");

  REQUIRE_THROWS_AS(match->span("y"), VariableNotFoundException);
}

}  // namespace rematch::testing
