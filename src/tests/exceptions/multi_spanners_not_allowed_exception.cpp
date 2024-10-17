#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>

namespace REmatch::testing {

TEST_CASE(
    "raise an exception when passing a REQL with multi spans to a Query") {
  std::vector<std::string> patterns = {
      "!x{a}+",
      "!x{a}!x{a}",
      "!y{!z{a}}!x{!z{a}}",
      "a|!x{b}",
  };
  for (const auto& pattern : patterns) {
    INFO(pattern);
    REQUIRE_THROWS_AS(reql(pattern), MultiSpannersNotAllowedException);
  }
}

}  // namespace REmatch::testing
