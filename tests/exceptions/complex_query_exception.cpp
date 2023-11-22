#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "mediator/mediator.hpp"

namespace rematch::testing {

extern char EOF_char;

TEST_CASE("an exception is thrown from ExtendedDetVA when the query is too complex") {
  std::string regex = std::string(100, 'a');
  auto parser = Parser(regex);
  auto document = std::string(100, 'a');
  document += EOF_char;

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();
  auto search_dfa = SearchDFA(logical_va);

  // pass the max value to the extended det va
  auto dfa_state_checker = DFAStateLimitChecker({.max_deterministic_states = 100});
  auto extended_det_va = ExtendedDetVA(extended_va, dfa_state_checker);

  auto evaluate_mediator = [&]() {
    auto mediator =
        Mediator(search_dfa, extended_det_va, variable_catalog, document);
    mediator::Mapping* mapping = mediator.next();
    while (mapping != nullptr) {
      mapping = mediator.next();
    }
  };

  REQUIRE_THROWS_AS(evaluate_mediator(), ComplexQueryException);
}

TEST_CASE("an exception is thrown from SearchDFA when the query is too complex") {
  std::string regex = std::string(100, 'a');
  auto parser = Parser(regex);
  auto document = std::string(100, 'a');
  document += EOF_char;

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  auto extended_det_va = ExtendedDetVA(extended_va);
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();

  // pass the max value to the searchdfa
  auto dfa_state_checker = DFAStateLimitChecker({.max_deterministic_states = 100});
  auto search_dfa = SearchDFA(logical_va, dfa_state_checker);

  auto evaluate_mediator = [&]() {
    auto mediator =
        Mediator(search_dfa, extended_det_va, variable_catalog, document);
    mediator::Mapping* mapping = mediator.next();
    while (mapping != nullptr) {
      mapping = mediator.next();
    }
  };

  REQUIRE_THROWS_AS(evaluate_mediator(), ComplexQueryException);
}

}  // namespace rematch::testing
