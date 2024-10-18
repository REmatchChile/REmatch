#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/exceptions.hpp>

#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("a exception is thrown when there is capture contains an anchor") {
  std::vector<std::string> invalid_regexes = {
    "!x{^a}",
    "!x{a$}",
    "!x{!y{^}}",
    "!x{!y{a}$}",
    "$a!x{a^}",
  };

  for (auto& regex : invalid_regexes) {
    SECTION("regex: " + regex) {
      REQUIRE_THROWS_AS(Parser(regex), REmatch::AnchorInsideCaptureException);
    }
  }
}

}  // namespace REmatch::testing
