#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include <REmatch/query_data.hpp>

#include "evaluation/document.hpp"
#include "exceptions/complex_query_exception.hpp"
#include "mediator/mediator/finditer_mediator.hpp"
#include "parsing/parser.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch::testing {

TEST_CASE(
    "an exception is thrown from SearchDFA when the query is too complex") {
  std::string regex = std::string(100, 'a');
  auto parser = Parser(regex);
  auto document_ = std::string(100, 'a');
  auto document = std::make_shared<Document>(document_);

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();

  auto segment_manager_creator = SegmentManagerCreator(logical_va, Flags::NONE, 100);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  // no need to evaluate the mediator, it searches for a segment in the constructor
  REQUIRE_THROWS_AS(FinditerMediator(regex_data, document,
                                     REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                                     REmatch::DEFAULT_MAX_DETERMINISTIC_STATES),
                    ComplexQueryException);
}

TEST_CASE(
    "a exception is thrown from ExtendedDetVA when the query is too complex") {
  // the regex contains an anchor, so the filtering process is skipped
  std::string regex = std::string(100, 'a') + '$';
  auto parser = Parser(regex);
  auto document_ = std::string(100, 'a');
  auto document = std::make_shared<Document>(document_);

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();

  auto segment_manager_creator = SegmentManagerCreator(logical_va, Flags::NONE, DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};

  auto evaluate_mediator = [&]() {
    auto mediator = FinditerMediator(regex_data, document,
                                     REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                                     100);
    mediator::Mapping* mapping = mediator.next();
    while (mapping != nullptr) {
      mapping = mediator.next();
    }
  };

  REQUIRE_THROWS_AS(evaluate_mediator(), ComplexQueryException);
}

}  // namespace REmatch::testing
