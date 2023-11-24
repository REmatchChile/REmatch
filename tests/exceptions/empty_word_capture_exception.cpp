#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"

namespace rematch::testing {

TEST_CASE("trying to capture an empty span raises an exception") {
  std::string regex = "!empty{a*}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::EmptyWordCaptureException);
}

TEST_CASE("trying to capture an empty span with alternation raises an exception") {
  std::string regex = "!empty{a|b*}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::EmptyWordCaptureException);
}

}  // namespace rematch::testing
