#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "output_enumeration/ecs.hpp"
#include "exceptions/memory_limit_exceeded_exception.hpp"

namespace rematch::testing {

ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);

TEST_CASE(
    "if the number of mempool duplications exceeds the limit, an exception is "
    "raised") {
  auto ecs = new ECS();
  auto flags = Flags();

  int maximum_size = (pow(2, flags.max_mempool_duplications + 1) - 1) *
                     MEMORY_POOL_STARTING_SIZE;

  create_linked_list_node_of_depth(ecs, maximum_size - 1);
  REQUIRE_THROWS_AS(ecs->create_bottom_node(), REMatch::MemoryLimitExceededException);
}
}
