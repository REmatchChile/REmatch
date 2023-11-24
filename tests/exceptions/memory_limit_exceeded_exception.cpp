#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "output_enumeration/ecs.hpp"
#include "exceptions/memory_limit_exceeded_exception.hpp"

namespace rematch::testing {

ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);

TEST_CASE("a mempool duplication raises a memory exception by default") {
  auto ecs = new ECS();
  create_linked_list_node_of_depth(ecs, MEMORY_POOL_STARTING_SIZE - 1);
  REQUIRE_THROWS_AS(ecs->create_bottom_node(), REMatch::MemoryLimitExceededException);
}

}
