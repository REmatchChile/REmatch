#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"
#include "exceptions/invalid_escape_exception.hpp"
#include "exceptions/invalid_range_exception.hpp"
#include "exceptions/same_nested_variable_exception.hpp"
#include "parsing/parser.hpp"
#include "parsing/variable_catalog.hpp"

namespace rematch::testing {

TEST_CASE("using the same variable nested raises an exception") {
  std::string regex = "!x{!x{a}}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::SameNestedVariableException);
}

TEST_CASE("trying to capture an empty span raises an exception") {
  std::string regex = "!empty{a*}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::EmptyWordCaptureException);
}

TEST_CASE("using more variables than the maximum raises an exception") {
  std::stringstream regex_stream;
  for (int i = 0; i < MAX_VARS + 1; i++) {
    regex_stream << "!x" << i << "{a}";
  }
  std::string regex = regex_stream.str();

  REQUIRE_THROWS_AS(Parser(regex), REMatch::VariableLimitExceededException);
}

TEST_CASE("accessing nonexistent variable raises an exception") {
  std::string regex = "!x{a}";
  auto parser = Parser(regex);
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();

  REQUIRE_THROWS_AS(variable_catalog->position("y"),
                    REMatch::VariableNotFoundInCatalogException);
}

TEST_CASE("invalid range in character class raises an exception") {
  std::string regex = "!x{[z-a]}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::InvalidRangeException);
}

TEST_CASE("invalid escape characters raise an exception") {
  std::string regex = "!x{\\e}";
  REQUIRE_THROWS_AS(Parser(regex), REMatch::InvalidEscapeException);
}

}  // namespace rematch::testing
