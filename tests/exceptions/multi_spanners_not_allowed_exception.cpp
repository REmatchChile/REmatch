#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "exceptions/multi_spanners_not_allowed_exception.hpp"
#include "library_interface/regex.hpp"

namespace rematch::testing {

TEST_CASE(
    "raise an exception when passing a REQL with multi spans to a Regex") {
  std::vector<std::string> patterns = {
      "!x{a}+",
      "!x{a}!x{a}",
      "!y{!z{a}}!x{!z{a}}",
      "a|!x{b}",
  };
  for (auto pattern : patterns) {
    INFO(pattern);
    REQUIRE_THROWS_AS(Regex(pattern),
                    MultiSpannersNotAllowedException);
  }
}

}  // namespace rematch::testing
