#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "mediator/mediator.hpp"

namespace rematch::testing {

TEST_CASE("an exception is thrown from SearchDFA when the query is too complex") {
  std::string regex = std::string(100, 'a');
  auto parser = Parser(regex);
  auto document = std::string(100, 'a');
  document += END_CHAR;

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();

  Flags flags{false, false, 8, 100};
  auto segment_manager_creator = SegmentManagerCreator(logical_va, flags);

  // no need to evaluate the mediator, it searches for a segment in the constructor
  REQUIRE_THROWS_AS(Mediator(extended_va, variable_catalog,
                             segment_manager_creator, std::move(document)),
                    ComplexQueryException);
}

TEST_CASE("a exception is thrown from ExtendedDetVA when the query is too complex") {
  // the regex contains an anchor, so the filtering process is skipped
  std::string regex = std::string(100, 'a') + '$';
  auto parser = Parser(regex);
  auto document = std::string(100, 'a');
  document += END_CHAR;

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();

  Flags flags{false, false, 8, 100};
  auto segment_manager_creator = SegmentManagerCreator(logical_va);

  auto evaluate_mediator = [&]() {
    auto mediator = Mediator(extended_va, variable_catalog,
                             segment_manager_creator, std::move(document), flags);
    mediator::Mapping* mapping = mediator.next();
    while (mapping != nullptr) {
      mapping = mediator.next();
    }
  };

  REQUIRE_THROWS_AS(evaluate_mediator(), ComplexQueryException);
}

}  // namespace rematch::testing
