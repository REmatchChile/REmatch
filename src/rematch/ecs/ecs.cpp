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

ECSNode *ECS::create_union_of_two_non_output_nodes(ECSNode *node_1, ECSNode *node_2, bool garbage_left) {

   /*****************/
   /*      U        */
   /*     / \       */
   /*    /   \      */
   /*   /     U     */
   /*  /     / \    */
   /* / U   /   U   */
   /* |/ \ /   / \  */
   /* o1  O   o2  O */
   /*****************/

    ECSNode *u2 = garbage_collector_.alloc(ECSNodeType::kUnion, node_1->right_node(), node_2->right_node());
    node_1->right_node()->inc_ref_count();
    node_2->right_node()->inc_ref_count();
    ECSNode *u1 = garbage_collector_.alloc(ECSNodeType::kUnion, node_2->left_node(), u2);
    node_2->left_node()->inc_ref_count();
    ECSNode *new_node = garbage_collector_.alloc(ECSNodeType::kUnion, node_1->left_node(), u1);
    node_1->left_node()->inc_ref_count();

    node_1->dec_ref_count();
    node_2->dec_ref_count();

    // Maybe node_1 and node_2 end up being useless
    if(garbage_left)
      try_mark_unused(node_1);
    try_mark_unused(node_2);
    return new_node;
}

} // namespace rematch
