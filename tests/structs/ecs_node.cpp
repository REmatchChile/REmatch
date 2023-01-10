#include <catch2/catch_test_macros.hpp>

#include "structs/ecs/ecs_node.hpp"
//#include "data/search.cpp"

namespace rematch::testing {


TEST_CASE("is_bottom works with bottom nodes",
          "[evaluation]") {
  ECSNode *node = new ECSNode(ECSNodeType::kBottom);
  REQUIRE(node->is_bottom());
}

TEST_CASE("is_bottom does not work with non bottom nodes",
          "[evaluation]") {
  for (ECSNodeType type : {ECSNodeType::kUnion,
                           ECSNodeType::kLabel}) {
    ECSNode *node = new ECSNode(type);
    REQUIRE(!node->is_bottom());
  }
}

TEST_CASE("label stores variable_markers correctly",
          "[evaluation]") {
  for (int value : {0, 3, 7, 12, 500, 1000}) {
    ECSNode *node = new ECSNode(ECSNodeType::kLabel);
  }
}


}  // namespace rematch::testing
