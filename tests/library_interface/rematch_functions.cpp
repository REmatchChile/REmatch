#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "library_interface/rematch.hpp"

namespace rematch::testing {
using namespace REMatch;

void run_client_test(std::unique_ptr<MatchIterator>& match_iterator,
                     std::vector<DummyMapping> expected_matches);

TEST_CASE("find function returns the correct match") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}$";
  std::unique_ptr<Match> match = findone(pattern, document);

  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == Span{3, 6});
}

TEST_CASE("match obtained with find returns the correct group") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}$";
  std::unique_ptr<Match> match = findone(pattern, document);

  REQUIRE(match->span("x") == Span{3, 6});
  REQUIRE(match->group("x") == "rty");
}

TEST_CASE("findall function returns the correct matches") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";
  std::vector<Match> matches = findall(pattern, document);

  std::vector<Span> expected_matches = {{0, 3}, {1, 4}, {2, 5}, {3, 6}};

  REQUIRE(matches.size() == expected_matches.size());

  for (size_t i = 0; i < expected_matches.size(); i++) {
    REQUIRE(matches[i].span("x") == expected_matches[i]);
  }
}

TEST_CASE("matches obtained with findall return the correct groups") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";
  std::vector<Match> matches = findall(pattern, document);

  std::vector<std::string> expected_groups = {"qwe", "wer", "ert", "rty"};

  REQUIRE(matches.size() == expected_groups.size());

  for (size_t i = 0; i < expected_groups.size(); i++) {
    REQUIRE(matches[i].group("x") == expected_groups[i]);
  }
}

TEST_CASE("the iterator obtained with finditer returns the correct matches") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";
  std::unique_ptr<MatchIterator> iterator = finditer(pattern, document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 3}}}),
      DummyMapping({{"x", {1, 4}}}),
      DummyMapping({{"x", {2, 5}}}),
      DummyMapping({{"x", {3, 6}}}),
  };

  run_client_test(iterator, expected_matches);
}

TEST_CASE("the matches obtained with finditer return the correct groups") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";
  std::unique_ptr<MatchIterator> iterator = finditer(pattern, document);

  std::vector<std::string> expected_groups = {"qwe", "wer", "ert", "rty"};

  std::unique_ptr<Match> match = iterator->next();

  while (match != nullptr) {
    std::string group = match->group("x");
    auto it = std::find(expected_groups.begin(), expected_groups.end(), group);

    REQUIRE(it != expected_groups.end());
    expected_groups.erase(it);

    match = iterator->next();
  }

  REQUIRE(expected_groups.empty());
}

}  // namespace rematch::testing
