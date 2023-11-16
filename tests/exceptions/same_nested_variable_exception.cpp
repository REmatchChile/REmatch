#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"
#include "exceptions/same_nested_variable_exception.hpp"

namespace rematch::testing {

TEST_CASE("using the same variable nested raises an exception") {
  std::string regex = "!x{!x{a}}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::SameNestedVariableException);
}

}  // namespace rematch::testing
