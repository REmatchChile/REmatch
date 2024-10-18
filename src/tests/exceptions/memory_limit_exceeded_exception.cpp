#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "output_enumeration/ecs.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

TEST_CASE(
    "if the number of mempool duplications exceeds the limit, an exception is "
    "raised") {
  auto ecs = new ECS(REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS);

  int maximum_size =
      (pow(2, REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS + 1) - 1) *
      MEMORY_POOL_STARTING_SIZE;

  create_linked_list_node_of_depth(ecs, maximum_size - 1);
  REQUIRE_THROWS_AS(ecs->create_bottom_node(),
                    REmatch::MemoryLimitExceededException);
}
}  // namespace REmatch::testing
