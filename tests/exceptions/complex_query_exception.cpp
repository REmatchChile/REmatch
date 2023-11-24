#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "mediator/mediator.hpp"

namespace rematch::testing {

TEST_CASE("a exception is thrown when the query is too complex") {
  std::string regex = "!x{" + std::string(500, 'a') + "}";
  auto parser = Parser(regex);
  auto document = std::string(1000, 'a');
  document += END_CHAR;

  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  auto extended_det_va = ExtendedDetVA(extended_va);
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(logical_va);

  Mediator mediator = Mediator(extended_det_va, variable_catalog, segment_manager_creator, document);

  auto evaluate_mediator = [&]() {
    mediator::Mapping* mapping = mediator.next();
    while (mapping != nullptr) {
      mapping = mediator.next();
    }
  };

  REQUIRE_THROWS_AS(evaluate_mediator(), ComplexQueryException);
}

}  // namespace rematch::testing
