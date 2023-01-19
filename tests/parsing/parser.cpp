#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "parsing/parser.hpp"

namespace rematch::testing {
  TEST_CASE("the regex 'a' is parsed correctly into a LogicalVA.") {
  Parser parser = Parser("a");
  //rematch::LogicalVA va = parser.doParse("a");
  //INFO("" << va);
  //INFO("" << va.accepting_state_ -> filters.size() );
  //INFO("" << va.init_state_-> filters.size() );
  //INFO("" << va.init_state_-> filters.front() );
  REQUIRE(false);
  }

}  // namespace rematch::testing
