#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"
#include "exceptions/invalid_escape_exception.hpp"

namespace rematch::testing {

TEST_CASE("invalid escape characters raise an exception") {
  std::string regex = "!x{\\e}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::InvalidEscapeException);
}

}  // namespace rematch::testing
