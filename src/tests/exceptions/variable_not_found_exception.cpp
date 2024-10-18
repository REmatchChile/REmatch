#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "mediator/mediator/finditer_mediator.hpp"
#include "utils/query_data.hpp"

#include <REmatch/REmatch.hpp>
namespace REmatch::testing {
using namespace REmatch;

TEST_CASE(
    "the mediator throws an exception when the variable is not in the regex") {
  Parser parser = Parser("!x{a}");
  std::string document_ = "a";
  auto document = std::make_shared<Document>(document_);

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(
      logical_va, Flags::NONE, REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  auto mediator = FinditerMediator(regex_data, document,
                                   REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS);

  auto mapping = mediator.next();
  REQUIRE_THROWS_AS(mapping->get_span_of_variable("y"),
                    VariableNotFoundException);
}

TEST_CASE(
    "the regex object raises an exception when the variable is not in the "
    "regex") {
  auto query = reql("!x{a}");
  auto match = query.findone("a");

  REQUIRE_THROWS_AS(match.span("y"), VariableNotFoundException);
}

}  // namespace REmatch::testing
