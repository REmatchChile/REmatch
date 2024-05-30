#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/multi_match_iterator.hpp"
#include "library_interface/multi_query.hpp"

namespace rematch::testing {

TEST_CASE("multi match iterator returns the correct variables") {
  std::string pattern = "(a!x{a} !y{a})+";
  std::string document = "aa aaa aa a";

  auto regex = REMatch::MultiQuery(pattern);
  std::unique_ptr<REMatch::MultiMatchIterator> iterator =
      regex.finditer(document);

  REQUIRE(iterator->variables() == std::vector<std::string>{"x", "y"});
}

TEST_CASE("multi match iterator returns the correct matches") {
  std::string pattern = "(^|[^\\w])!x{\\w+} !x{\\w+}($|[^\\w])";
  std::string document = "one two three";

  auto regex = REMatch::MultiQuery(pattern);
  std::unique_ptr<REMatch::MultiMatchIterator> iterator =
      regex.finditer(document);

  auto match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{0, 3}, {4, 7}});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{4, 7}, {8, 13}});

  REQUIRE(iterator->next() == nullptr);
}

}  // namespace rematch::testing
