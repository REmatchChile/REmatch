#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

TEST_CASE("flag LINE_BY_LINE works correctly") {
  std::string pattern = "!x{([A-Z][a-z]+\\n)+}";
  std::string document = "Anna\nJuan\nPaul\nLuis\n";
  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 5}}}), DummyMapping({{"x", {5, 10}}}),
      DummyMapping({{"x", {10, 15}}}), DummyMapping({{"x", {15, 20}}})};

  Flags flags = Flags::LINE_BY_LINE;
  REmatch::Query regex = REmatch::reql(pattern, flags);
  auto match_generator = regex.finditer(document);

  run_client_test(match_generator, expected_matches);
}

TEST_CASE(
    "flag max_mempool_duplications allows creating more nodes correctly") {
  auto ecs = new ECS(1000);
  create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1);
  REQUIRE_NOTHROW(
      create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1));
}

}  // namespace REmatch::testing
