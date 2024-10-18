#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("invalid range in character class raises an exception") {
  std::string regex = "!x{[z-a]}";
  REQUIRE_THROWS_AS(Parser(regex), REmatch::InvalidRangeException);
}

}  // namespace REmatch::testing
