#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../evaluation/mapping_helpers.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "mediator/findone_mediator/findone_mediator.hpp"
#include "mediator/mediator_constructor.hpp"
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("the findone mediator returns nullptr if there are no matches") {
  std::string regex = "!x{ab}";
  auto document = std::make_shared<Document>("bbb");
  Parser parser(regex);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va(logical_va);
  extended_va.clean_for_determinization();

  QueryData regex_data(std::move(extended_va), parser.get_variable_catalog(), logical_va,
                       Flags::NONE, DEFAULT_MAX_DETERMINISTIC_STATES);

  std::unique_ptr<Mediator> mediator =
      MediatorConstructor::create_findone_mediator(regex_data, document);

  auto mapping = mediator->next();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the findone mediator returns the correct match") {
  std::string regex = "!x{b+}";
  auto document = std::make_shared<Document>("abba");
  Parser parser(regex);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va(logical_va);
  extended_va.clean_for_determinization();

  auto regex_data = QueryData(std::move(extended_va), parser.get_variable_catalog(), logical_va,
                              Flags::NONE, DEFAULT_MAX_DETERMINISTIC_STATES);
  std::unique_ptr<Mediator> mediator =
      MediatorConstructor::create_findone_mediator(regex_data, document);
  auto mapping = mediator->next();
  std::map<int, Span> mapping_map = mapping->get_spans_map();
  REQUIRE(mapping_map == std::map<int, Span>{{0, {1, 2}}});
}

}  // namespace REmatch::testing
