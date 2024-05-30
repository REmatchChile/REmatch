#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "library_interface/rematch.hpp"

namespace rematch::testing {

void run_client_test(std::unique_ptr<MatchIterator>& match_iterator,
                     std::vector<DummyMapping> expected_matches);
void create_linked_list_node_of_depth(ECS* ecs, int depth);

TEST_CASE("flag line_by_line works correctly") {
  std::string pattern = "!x{([A-Z][a-z]+\\n)+}";
  std::string document = "Anna\nJuan\nPaul\nLuis\n";
  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 5}}}), DummyMapping({{"x", {5, 10}}}),
      DummyMapping({{"x", {10, 15}}}), DummyMapping({{"x", {15, 20}}})};

  Flags flags{true, false, 8, 1000};
  REMatch::Query regex = REMatch::compile(pattern, flags);
  std::unique_ptr<REMatch::MatchIterator> match_iterator =
      regex.finditer(document);

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE(
    "flag max_mempool_duplications allows creating more nodes correctly") {
  Flags flags{false, false, 1, 1000};
  auto ecs = new ECS(flags);
  create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1);
  REQUIRE_NOTHROW(
      create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1));
}

}  // namespace rematch::testing
