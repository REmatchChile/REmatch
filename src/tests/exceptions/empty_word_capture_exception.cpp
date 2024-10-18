#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("trying to capture an empty span raises an exception") {
  std::string regex = "!empty{a*}";
  REQUIRE_THROWS_AS(Parser(regex), REmatch::EmptyWordCaptureException);
}

TEST_CASE("trying to capture an empty span with alternation raises an exception") {
  std::string regex = "!empty{a|b*}";
  REQUIRE_THROWS_AS(Parser(regex), REmatch::EmptyWordCaptureException);
}

}  // namespace REmatch::testing
