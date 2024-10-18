#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("using the same variable nested raises an exception") {
  std::string regex = "!x{!x{a}}";
  REQUIRE_THROWS_AS(Parser(regex), REmatch::SameNestedVariableException);
}

}  // namespace REmatch::testing
