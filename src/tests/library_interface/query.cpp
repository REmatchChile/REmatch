#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "evaluation/start_end_chars.hpp"

#include "REmatch/REmatch.hpp"

namespace REmatch::testing {

TEST_CASE("find method returns the first match correctly") {
  std::string pattern = "!x{ab}";
  auto query = reql(pattern);
  auto match = query.findone("abab");

  REQUIRE(match->span("x") == Span(0, 2));
}

TEST_CASE("finditer method returns the iterator correctly") {
  std::string pattern = "!x{ab}";
  std::string document = "abab";
  document += END_CHAR;
  auto query = reql(pattern);
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();
  auto end = match_generator.end();

  REQUIRE(it != end);
  auto match = *it;
  REQUIRE(match.span("x") == Span(0, 2));

  REQUIRE(++it != end);
  match = *it;
  REQUIRE(match.span("x") == Span(2, 4));

  REQUIRE(++it == end);
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

TEST_CASE("standard check lbl") {
  std::string pattern = "!x{..}";
  std::string document = "0\n2\n4\n7";
  auto query = reql(pattern, Flags::LINE_BY_LINE);

  REQUIRE_FALSE(query.check(document));
}

TEST_CASE("stream check lbl") {
  std::string pattern = "!x{..}";
  std::string document = "0\n2\n4\n7";
  std::stringstream document_stream(document);
  auto reader = std::make_unique<FStreamReader>(document_stream);
  auto query = reql(pattern, Flags::LINE_BY_LINE);

  REQUIRE_FALSE(query.check(reader.get()));
}

TEST_CASE("standard no capture") {
  std::string pattern = "a";
  std::string document = "aabbaa";

  auto query = reql(pattern);
  auto matches = query.findall(document);

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

TEST_CASE("standard no capture lbl") {
  std::string pattern = "a";
  std::string document = "aabbaa";

  auto query = reql(pattern, Flags::LINE_BY_LINE);
  auto matches = query.findall(document);

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

TEST_CASE("stream no capture") {
  std::string pattern = "a";
  std::string document = "aabbaa";
  std::stringstream document_stream(document);
  auto reader = std::make_unique<FStreamReader>(document_stream);
  auto query = reql(pattern);
  auto matches = query.findall(reader.get());

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

TEST_CASE("stream no capture lbl") {
  std::string pattern = "a";
  std::string document = "aabbaa";
  std::stringstream document_stream(document);
  auto reader = std::make_unique<FStreamReader>(document_stream);
  auto query = reql(pattern, Flags::LINE_BY_LINE);
  auto matches = query.findall(reader.get());

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

}  // namespace REmatch::testing
