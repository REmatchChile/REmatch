#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../tests_utils/tests_utils.hpp"
#include "mediator/mediator_constructor.hpp"
#include "utils/query_data.hpp"

namespace REmatch::testing {

TEST_CASE("the mediator throws an exception when the variable is not in the regex") {
  Parser parser = Parser("!x{a}");
  std::string document_ = "a";
  auto document = std::make_shared<Document>(document_);

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();

  QueryData regex_data(std::move(extended_va), parser.get_variable_catalog(), logical_va,
                       Flags::NONE, DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                       DEFAULT_MAX_DETERMINISTIC_STATES);
  auto mediator = MediatorConstructor::create_finditer_mediator(regex_data, document);

  auto mapping = mediator->next();
  REQUIRE(mapping != nullptr);
  REQUIRE_THROWS_AS(mapping->get_span_of_variable(1), VariableNotFoundException);
}

TEST_CASE(
    "the regex object raises an exception when the variable is not in the "
    "regex") {
  auto query = reql("!x{a}");
  auto match = query.findone("a");

  REQUIRE_THROWS_AS(match->span("y"), VariableNotFoundException);
}

}  // namespace REmatch::testing
