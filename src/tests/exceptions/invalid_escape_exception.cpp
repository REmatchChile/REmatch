#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("invalid escape characters raise an exception") {
  std::string regex = "!x{\\e}";
  REQUIRE_THROWS_AS(Parser(regex), REmatch::InvalidEscapeException);
}

}  // namespace REmatch::testing
