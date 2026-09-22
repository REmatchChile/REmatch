#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

TEST_CASE("flag LINE_BY_LINE works correctly") {
  std::string pattern = "!x{.{4,}}";
  std::string document = "Anna\nJuan\nPaul\nLuis\n";
  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 4}}}), DummyMapping({{"x", {5, 9}}}), DummyMapping({{"x", {10, 14}}}),
      DummyMapping({{"x", {15, 19}}})};

  Flags flags = Flags::LINE_BY_LINE;
  Query regex = reql(pattern, flags);
  auto match_generator = regex.finditer(document);

  run_client_test(match_generator, expected_matches);
}

TEST_CASE("flag LINE_BY_LINE works correctly b") {
  std::string pattern = "^!x{a}";
  std::string document = "a\naaa\nab\nbb\na";
  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 1}}}), DummyMapping({{"x", {2, 3}}}), DummyMapping({{"x", {6, 7}}}),
      DummyMapping({{"x", {12, 13}}})};

  Flags flags = Flags::LINE_BY_LINE;
  Query regex = reql(pattern, flags);
  auto match_generator = regex.finditer(document);

  run_client_test(match_generator, expected_matches);
}

TEST_CASE("flag LINE_BY_LINE works correctly c") {
  std::string pattern = "^!x{a}";
  std::string document = "b\naaa\nab\nbb\na";
  std::vector<DummyMapping> expected_matches = {DummyMapping({{"x", {2, 3}}}),
                                                DummyMapping({{"x", {6, 7}}}),
                                                DummyMapping({{"x", {12, 13}}})};

  Flags flags = Flags::LINE_BY_LINE;
  Query regex = reql(pattern, flags);
  auto match_generator = regex.finditer(document);

  run_client_test(match_generator, expected_matches);
}

TEST_CASE("flag LINE_BY_LINE works correctly using findone") {
  std::string pattern = "^!x{a}";
  std::string document = "b\naaa\nab\nbb\na";

  Flags flags = Flags::LINE_BY_LINE;
  Query regex = reql(pattern, flags);

  REQUIRE_NOTHROW([&]() {
    auto match = regex.findone(document);
    REQUIRE(match.has_value());
    REQUIRE(match->span("x") == Span{2, 3});
  }());
}

TEST_CASE("flag LINE_BY_LINE works correctly using findone and anchors") {
  std::string pattern = "^!x{a}";
  std::string document = "a\naaa\nab\nbb\na";

  Flags flags = Flags::LINE_BY_LINE;
  Query regex = reql(pattern, flags);

  REQUIRE_NOTHROW([&]() {
    auto match = regex.findone(document);
    REQUIRE(match.has_value());
    REQUIRE(match->span("x") == Span{0, 1});
  }());
}

TEST_CASE("flag max_mempool_duplications allows creating more nodes correctly") {
  auto ecs = ECS(1000);
  create_linked_list_node_of_depth(&ecs, MEMORY_POOL_STARTING_SIZE - 1);
  REQUIRE_NOTHROW(create_linked_list_node_of_depth(&ecs, MEMORY_POOL_STARTING_SIZE - 1));
}

}  // namespace REmatch::testing
