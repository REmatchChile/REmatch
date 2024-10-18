#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/node_manager.hpp"

namespace REmatch::testing {

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
    ECSNode *bottom_node = ecs->create_bottom_node();
    ensure_node_has_correct_odepth(bottom_node);
    return bottom_node;
  }

  ECSNode *create_extend_node(
      ECS *ecs,
      int variable_annotations, int document_position
    ) {
    static ECSNode *base_node = create_bottom_node(ecs);
    ECSNode* output_node = ecs->create_extend_node(
        base_node, variable_annotations, document_position);
    ensure_node_has_correct_odepth(output_node);
    return output_node;
  }

  ECSNode *create_union_node(ECS *ecs) {
    static ECSNode *base_node = create_bottom_node(ecs);
    ECSNode *output_node = ecs->create_union_node(base_node, base_node);
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
          break;
        case 1:
          create_union_node(ecs);
      }
    }
  }

  void fill_one_mempool_with_free_bottom_nodes(ECS *ecs) {
    for (int i = 0; i < (int) MEMORY_POOL_STARTING_SIZE; i++) {
      ECSNode* bottom_node = create_bottom_node(ecs);
      ecs->pin_node(bottom_node);
      ecs->unpin_node(bottom_node);
    }
  }

  TEST_CASE("When all nodes are returned, no extra nodes are used: Bottom node") {
    static ECS *ecs = new ECS();
    fill_one_mempool_with_free_bottom_nodes(ecs);
    REQUIRE(ecs->get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);
    fill_one_mempool_with_free_bottom_nodes(ecs);
    REQUIRE(ecs->get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);
  }

  ECSNode* create_mempool_sized_linked_list_of_label_nodes(ECS *ecs) {
    ECSNode* current_node = create_bottom_node(ecs);
    for (int i = 0; i < (int) MEMORY_POOL_STARTING_SIZE - 1; i++) {
      ECSNode* next_node = ecs->create_extend_node(current_node, i, i);
      current_node = next_node;
    }
    return current_node;
  }

  // garbage_collector -> node_manager, return_node -> unpin, bottom_node -> pin_bottom_node,

  TEST_CASE("When all nodes are returned, no extra nodes are used: label node") {
    static ECS *ecs = new ECS();
    ECSNode* linked_list = create_mempool_sized_linked_list_of_label_nodes(ecs);
    ecs->pin_node(linked_list);
    ecs->unpin_node(linked_list);
    create_mempool_sized_linked_list_of_label_nodes(ecs);
    REQUIRE(ecs->get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);
  }

  int log2_rounded_up(int amount_of_nodes) {
    int log2 = 0;
    while (amount_of_nodes > 0) {
      amount_of_nodes /= 2;
      log2++;
    }
    return log2;
  }

  ECSNode* create_mempool_sized_binary_tree_of_union_nodes(ECS *ecs) {
    int height_of_tree = log2_rounded_up(MEMORY_POOL_STARTING_SIZE);
    int theoretical_amount_of_nodes = (1 << height_of_tree) - 1;
    int amount_of_bottom_nodes = 1 << height_of_tree;
    int starting_point_of_bottom_nodes = theoretical_amount_of_nodes - amount_of_bottom_nodes;

    ECSNode **nodes = new ECSNode*[MEMORY_POOL_STARTING_SIZE];
    for (int i = starting_point_of_bottom_nodes;
             i < (int) MEMORY_POOL_STARTING_SIZE;
             i++) {
      nodes[i] = ecs->create_bottom_node();
      ecs->pin_node(nodes[i]);
    }
    int amount_of_nodes_of_previous_layer = amount_of_bottom_nodes;
    int starting_point_of_previous_layer = starting_point_of_bottom_nodes;
    while (amount_of_nodes_of_previous_layer > 1) {
      int amount_of_nodes_of_current_layer = amount_of_nodes_of_previous_layer / 2;
      int starting_point_of_current_layer = starting_point_of_previous_layer - amount_of_nodes_of_current_layer;
      for (int i = starting_point_of_current_layer;
               i < starting_point_of_previous_layer;
               i++) {
        // if the node i*2 or i*2 + 1 is higher than MEMORY_POOL_STARTING_SIZE,
        // we use the node MEMORY_POOL_STARTING_SIZE - 1 instead.
        int left_child_index = i*2;
        int right_child_index = i*2 + 1;
        if (left_child_index >= (int) MEMORY_POOL_STARTING_SIZE) {
          left_child_index = MEMORY_POOL_STARTING_SIZE - 1;
        }
        if (right_child_index >= (int) MEMORY_POOL_STARTING_SIZE) {
          right_child_index = MEMORY_POOL_STARTING_SIZE - 1;
        }
        nodes[i] = ecs->create_union_node(nodes[left_child_index], nodes[right_child_index]);
        ecs->pin_node(nodes[i]);
      }
      amount_of_nodes_of_previous_layer = amount_of_nodes_of_current_layer;
      starting_point_of_previous_layer = starting_point_of_current_layer;
    }
    for (int i = 1; i < (int) MEMORY_POOL_STARTING_SIZE; i++) {
      ecs->unpin_node(nodes[i]);
    }
    return nodes[0];
  }

  TEST_CASE("When all nodes are returned, no extra nodes are used: union node") {
    static ECS *ecs = new ECS(1000);
    ECSNode* binary_tree = create_mempool_sized_linked_list_of_label_nodes(ecs);
    ecs->pin_node(binary_tree);
    ecs->unpin_node(binary_tree);
    REQUIRE(ecs->get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);
    create_mempool_sized_linked_list_of_label_nodes(ecs);
    REQUIRE(ecs->get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);
    create_mempool_sized_linked_list_of_label_nodes(ecs);
    REQUIRE(ecs->get_amount_of_nodes_used() == 2 * MEMORY_POOL_STARTING_SIZE);
  }

}  // namespace REmatch::testing
