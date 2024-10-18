#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {
using namespace REmatch;

TEST_CASE("find function returns the correct match") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}$";

  auto query = reql(pattern);
  auto match = query.findone(document);

  REQUIRE(match.span("x") == Span{3, 6});
}

TEST_CASE("match obtained with find returns the correct group") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}$";

  auto query = reql(pattern);
  auto match = query.findone(document);

  REQUIRE(match.span("x") == Span{3, 6});
  REQUIRE(match.group("x") == "rty");
}

TEST_CASE("findall function returns the correct matches") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";

  auto query = reql(pattern);
  auto matches = query.findall(document);

  std::vector<Span> expected_matches = {{0, 3}, {1, 4}, {2, 5}, {3, 6}};

  REQUIRE(matches.size() == expected_matches.size());

  for (size_t i = 0; i < expected_matches.size(); i++) {
    REQUIRE(matches[i].span("x") == expected_matches[i]);
  }
}

TEST_CASE("matches obtained with findall return the correct groups") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";

  auto query = reql(pattern);
  auto matches = query.findall(document);

  std::vector<std::string> expected_groups = {"qwe", "wer", "ert", "rty"};

  REQUIRE(matches.size() == expected_groups.size());

  for (size_t i = 0; i < expected_groups.size(); i++) {
    REQUIRE(matches[i].group("x") == expected_groups[i]);
  }
}

TEST_CASE("the iterator obtained with finditer returns the correct matches") {
  std::string document = "qwerty";
  std::string pattern = "!x{.{3}}";

  auto query = reql(pattern);
  auto iterator = query.finditer(document);

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

  auto query = reql(pattern);
  auto match_generator = query.finditer(document);

  std::vector<std::string> expected_groups = {"qwe", "wer", "ert", "rty"};

  for (auto& match : match_generator) {
    auto group = match.group("x");
    auto it = std::find(expected_groups.begin(), expected_groups.end(), group);

    REQUIRE(it != expected_groups.end());
    expected_groups.erase(it);
  }

  REQUIRE(expected_groups.empty());
}

}  // namespace REmatch::testing
