#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "parsing/parser.hpp"

namespace rematch::testing {
  TEST_CASE("the regex 'a' is parsed correctly into a LogicalVA.") {
  Parser parser = Parser("a");
  rematch::LogicalVA va = parser.get_logical_va();
  INFO(va.initial_state()->filters.back()->charclass);
  REQUIRE(va.initial_state()->filters.back()->charclass.contains('a'));
  //REQUIRE(false);
  }

}  // namespace rematch::testing
