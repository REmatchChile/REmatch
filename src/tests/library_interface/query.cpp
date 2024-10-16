#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "evaluation/start_end_chars.hpp"

#include <REmatch/REmatch.hpp>

namespace REmatch::testing {
using namespace REmatch::library_interface;

TEST_CASE("find method returns the first match correctly") {
  std::string pattern = "!x{ab}";
  auto query = reql(pattern);
  std::unique_ptr<Match> match = query.findone("abab");

  REQUIRE(match->span("x") == Span(0, 2));
}

TEST_CASE("finditer method returns the iterator correctly") {
  std::string pattern = "!x{ab}";
  std::string document = "abab";
  document += END_CHAR;
  auto query = reql(pattern);
  auto match_iterator = query.finditer(document);

  std::unique_ptr<Match> match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == Span(0, 2));

  match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == Span(2, 4));

  match = match_iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("check method returns true when there is an output") {
  std::string pattern = "!x{ab}";
  std::string document = "abab";
  auto query = reql(pattern);

  bool has_output = query.check(document);
  REQUIRE(has_output);
}

TEST_CASE("check method returns false when there is no output") {
  std::string pattern = "!x{baa}";
  std::string document = "abab";
  auto query = reql(pattern);

  bool has_output = query.check(document);
  REQUIRE_FALSE(has_output);
}

}  // namespace REmatch::testing