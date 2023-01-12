#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "ecs/ecs.hpp"

namespace rematch::testing {

  int compute_odepth_of_ecs_node(ECSNode *node) {
    if (node->is_output() || node->is_bottom()) {
      return 0;
    } else {
      return 1 + compute_odepth_of_ecs_node(node->next());
    }
  }

  void ensure_node_has_correct_odepth(ECSNode *node) {
    REQUIRE(compute_odepth_of_ecs_node(node) <= 1);
  }
  
  ECSNode *create_bottom_node(ECS *ecs) {
    ECSNode *bottom_node = ecs->bottom_node();
    ensure_node_has_correct_odepth(bottom_node);
    return bottom_node;
  }

  ECSNode *create_extend_node(
      ECS *ecs,
      int variable_annotations, int document_position
    ) {
    static ECSNode *base_node = create_bottom_node(ecs);
    ECSNode* output_node = ecs->extend(
        base_node, variable_annotations, document_position);
    ensure_node_has_correct_odepth(output_node);
    return output_node;
  }

  ECSNode *create_union_node(ECS *ecs) {
    static ECSNode *base_node = create_bottom_node(ecs);
    ECSNode *output_node = ecs->unite(base_node, base_node);
    ensure_node_has_correct_odepth(output_node);
    return output_node;
  }

  TEST_CASE("Nodes created by ECS have correct odepth of 1") {
    static ECS *ecs = new ECS();
    int amount_of_nodes_created = 14;
    for (int i = 0; i < amount_of_nodes_created; i++) {
      switch (i % 2) {
        case 0:
          create_extend_node(ecs, i, i);
        case 1:
          create_union_node(ecs);
      }
    }
  }

}  // namespace rematch::testing
