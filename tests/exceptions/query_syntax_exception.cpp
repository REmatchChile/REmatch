#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"
#include "exceptions/query_syntax_exception.hpp"

namespace rematch::testing {

TEST_CASE("other parsing errors raise a syntax exception") {
  std::vector<std::string> invalid_regexes = {
    "!{a}",
    "!123{a}",
    "a{i}",
    "!x+{a}",
    "[a-]",
    "!x{ab)}",
    "!x{}"
  };

  for (auto& regex : invalid_regexes) {
    SECTION("invalid regex: " + regex) {
      REQUIRE_THROWS_AS(Parser(regex), REMatch::QuerySyntaxException);
    }
  }
}

}  // namespace rematch::testing
