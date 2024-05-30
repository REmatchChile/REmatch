#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/query.hpp"

namespace rematch::testing {
using namespace REMatch::library_interface;

TEST_CASE("find method returns the first match correctly") {
  std::string pattern = "!x{ab}";
  auto regex = Query(pattern);
  std::unique_ptr<Match> match = regex.findone("abab");

  REQUIRE(match->span("x") == Span(0, 2));
}

TEST_CASE("finditer method returns the iterator correctly") {
  std::string pattern = "!x{ab}";
  std::string document = "abab";
  document += END_CHAR;
  auto regex = Query(pattern);
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

TEST_CASE("check method returns true when there is an output") {
  std::string pattern = "!x{ab}";
  std::string document = "abab";
  auto regex = Query(pattern);

  bool has_output = regex.check(document);
  REQUIRE(has_output);
}

TEST_CASE("check method returns false when there is no output") {
  std::string pattern = "!x{baa}";
  std::string document = "abab";
  auto regex = Query(pattern);

  bool has_output = regex.check(document);
  REQUIRE_FALSE(has_output);
}

}  // namespace rematch::testing
