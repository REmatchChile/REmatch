#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "mediator/mediator.hpp"
#include "library_interface/regex.hpp"

namespace rematch::testing {
using namespace REMatch::library_interface;

TEST_CASE("find method returns the first match correctly") {
  std::string pattern = "!x{ab}";
  auto regex = Regex(pattern);
  std::unique_ptr<Match> match = regex.findone("abab");

  REQUIRE(match->span("x") == Span(0, 2));
}

TEST_CASE("finditer method returns the iterator correctly") {
  std::string pattern = "!x{ab}";
  std::string document = "abab";
  document += END_CHAR;
  auto regex = Regex(pattern);
  std::unique_ptr<MatchIterator> match_iterator = regex.finditer(document);

  std::unique_ptr<Match> match = match_iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == Span(0, 2));

  match = match_iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == Span(2, 4));

  match = match_iterator->next();
  REQUIRE(match == nullptr);
}

}  // namespace rematch::testing
