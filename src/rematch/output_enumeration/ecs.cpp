#include <iostream>
#include <memory>

#include "output_enumeration/ecs.hpp"
#include "ecs.hpp"

namespace REmatch {
inline namespace output_enumeration {

ECS::ECS(uint_fast32_t max_mempool_duplications) : node_manager(max_mempool_duplications) {}

size_t ECS::get_amount_of_nodes_used() const {
  return node_manager.get_amount_of_nodes_used();
}

size_t ECS::get_amount_of_nodes_reused() const {
  return node_manager.get_amount_of_nodes_reused();
}

ECSNode* ECS::create_bottom_node() {
  return node_manager.alloc(ECSNodeType::kBottom);
}

ECSNode *ECS::create_extend_node(ECSNode *node,
                     std::bitset<64> variable_markers, int document_position) {
  return node_manager.alloc(
      ECSNodeType::kLabel, node, nullptr,
      variable_markers, document_position);

}

ECSNode *ECS::pin_node(ECSNode *node) {
  node_manager.increase_ref_count(node);
  return node;
}

void ECS::unpin_node(ECSNode *node) {
  node_manager.decrease_ref_count(node);
}

ECSNode *ECS::create_union_node(ECSNode *node_1, ECSNode *node_2) {
  if (node_1->is_output()) {
    return node_manager.alloc(ECSNodeType::kUnion, node_1, node_2);
  } else if (node_2->is_output()) {
    return node_manager.alloc(ECSNodeType::kUnion, node_2, node_1);
  } else {
    return create_union_of_two_non_output_nodes(node_1, node_2);
  }
}

ECSNode *ECS::create_union_of_two_non_output_nodes(
  /*!       _\|/_
            (o o)
    +----oOO-{_}-OOo---------------------------------------+
    |          new                                         |
    |         /   \            The union of two nodes that |
    |        /    u2           are non output nodes creates|
    |       /      |\          the union that is shown on  |
    |      /       | u1        the left.                   |
    |     /        |/  \                                   |
    |    /         |    \                                  |
    |   /         /|     \                                 |
    |  /         / |      \                                |
    | /  n1     /  |   n2\ |                               |
    || /   \   /   | /    \|                               |
    |o1      O     o2      O                               |
    +----------------------------------------------------- +
  */
    ECSNode *node_1, ECSNode *node_2) {

    ECSNode *u1 = create_first_intermediate_union_node(node_1, node_2);
    ECSNode *u2 = create_second_intermediate_union_node(node_2, u1);
    ECSNode *new_node = create_union_of_output_and_intermediate_node(node_1, u2);
    return new_node;
}

ECSNode *ECS::create_first_intermediate_union_node(ECSNode *node_1,
                                                   ECSNode *node_2) {
    ECSNode *u1 = node_manager.alloc(ECSNodeType::kUnion,
                                           node_1->right_node(),
                                           node_2->right_node());
    return u1;
}

ECSNode *ECS::create_second_intermediate_union_node(ECSNode *node_2,
                                                    ECSNode *u1) {
    ECSNode *u2 = node_manager.alloc(ECSNodeType::kUnion,
                                           node_2->left_node(),
                                           u1);
    return u2;
}

ECSNode *ECS::create_union_of_output_and_intermediate_node(
    ECSNode *node_1, ECSNode *u2) {
    ECSNode *new_node = node_manager.alloc(ECSNodeType::kUnion,
                                                 node_1->left_node(), u2);
    return new_node;
}
}
}
