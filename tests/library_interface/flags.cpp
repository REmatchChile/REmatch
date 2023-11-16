#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "library_interface/rematch.hpp"
#include "regex"

namespace rematch::testing {

std::string get_mapping_info(DummyMapping mapping);
void run_client_test(MatchIterator& match_iterator,
                     std::vector<DummyMapping> expected_matches);
void create_linked_list_node_of_depth(ECS* ecs, int depth);

TEST_CASE("flag line_by_line works correctly") {
  std::string pattern = "!x{([A-Z][a-z]+\\n)+}";
  std::string document = "Anna\nJuan\nPaul\nLuis\n";
  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 5}}}), DummyMapping({{"x", {5, 10}}}),
      DummyMapping({{"x", {10, 15}}}), DummyMapping({{"x", {15, 20}}})};

  REMatch::Regex regex = REMatch::compile(pattern);
  Flags flags = {.line_by_line = true};
  REMatch::MatchIterator match_iterator = regex.finditer(document, flags);

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE(
    "flag max_mempool_duplications allows creating more nodes correctly") {
  auto ecs = new ECS({.max_mempool_duplications = 1});
  create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1);
  REQUIRE_NOTHROW(
      create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1));
}

}  // namespace rematch::testing
