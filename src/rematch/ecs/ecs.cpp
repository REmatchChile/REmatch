#include <iostream>
#include <memory>

#include "ecs.hpp"

namespace rematch {

ECSNode *ECS::extend(ECSNode *v, std::bitset<32> S, int64_t i) {
  return garbage_collector_.alloc(ECSNodeType::kLabel, v, nullptr, S, i);
}

ECSNode *ECS::unite(ECSNode *node_1, ECSNode *node_2, bool garbage_left) {
  if (node_1->is_output()) {
    return garbage_collector_.alloc(ECSNodeType::kUnion, node_1, node_2);
  } else if (node_2->is_output()) {
    return garbage_collector_.alloc(ECSNodeType::kUnion, node_2, node_1);
  } else {
    return create_union_of_two_non_output_nodes(node_1, node_2, garbage_left);
  }
}

ECSNode *ECS::create_union_of_two_non_output_nodes(
    ECSNode *node_1, ECSNode *node_2, bool garbage_left) {
   /*       _\|/_
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
    +-----------------------------------------------------*/

    ECSNode *u1 = create_first_intermediate_union_node(node_1, node_2);
    ECSNode *u2 = create_second_intermediate_union_node(node_2, u1);
    ECSNode *new_node = create_union_node(node_1, u2);

    rectify_node_references_after_union(node_1, node_2);
    check_if_previous_nodes_are_unused(node_1, node_2, garbage_left);
    return new_node;
}

ECSNode *ECS::create_first_intermediate_union_node(ECSNode *node_1,
                                                   ECSNode *node_2) {
    ECSNode *u1 = garbage_collector_.alloc(ECSNodeType::kUnion,
                                           node_1->right_node(),
                                           node_2->right_node());
    node_1->right_node()->inc_ref_count(); // TODO: Discuss delegating this
    node_2->right_node()->inc_ref_count(); // To ecs_node constructor
    return u1;
}

ECSNode *ECS::create_second_intermediate_union_node(ECSNode *node_2, 
                                                    ECSNode *u1) {
    ECSNode *u2 = garbage_collector_.alloc(ECSNodeType::kUnion,
                                           node_2->left_node(),
                                           u1);
    node_2->left_node()->inc_ref_count();
    return u2;
}

ECSNode *ECS::create_union_node(ECSNode *node_1, ECSNode *u2) {
    ECSNode *new_node = garbage_collector_.alloc(ECSNodeType::kUnion,
                                                 node_1->left_node(), u2);
    node_1->left_node()->inc_ref_count();
    return new_node;
}

void ECS::rectify_node_references_after_union(ECSNode *node_1, ECSNode *node_2) {
    node_1->dec_ref_count();
    node_2->dec_ref_count();
}

void ECS::check_if_previous_nodes_are_unused(ECSNode *node_1,
                                             ECSNode *node_2,
                                             bool garbage_left) {
    if(garbage_left)
      try_mark_unused(node_1);
    try_mark_unused(node_2);
}

} // namespace rematch
