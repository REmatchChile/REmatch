#include <iostream>
#include <memory>

#include "ecs.hpp"

namespace rematch {

ECSNode *ECS::extend(ECSNode *v, std::bitset<32> S, int64_t i) {
  ECSNode *v_prim = garbage_collector_.alloc(ECSNodeType::kLabel, v, nullptr, S, i);
  return v_prim;
}

ECSNode *ECS::unite(ECSNode *v1, ECSNode *v2, bool garbage_left) {
  ECSNode *v_prim;
  if (v1->is_output()) {
    v_prim = garbage_collector_.alloc(ECSNodeType::kUnion, v1, v2);
  } else if (v2->is_output()) {
    v_prim = garbage_collector_.alloc(ECSNodeType::kUnion, v2, v1);
  } else {
    ECSNode *u2 = garbage_collector_.alloc(ECSNodeType::kUnion, v1->right_node(), v2->right_node());
    v1->right_node()->inc_ref_count();
    v2->right_node()->inc_ref_count();
    ECSNode *u1 = garbage_collector_.alloc(ECSNodeType::kUnion, v2->left_node(), u2);
    v2->left_node()->inc_ref_count();
    v_prim = garbage_collector_.alloc(ECSNodeType::kUnion, v1->left_node(), u1);
    v1->left_node()->inc_ref_count();

    v1->dec_ref_count();
    v2->dec_ref_count();

    // Maybe v1 and v2 end up being useless
    if(garbage_left)
      try_mark_unused(v1);
    try_mark_unused(v2);
  }
  return v_prim;
}

} // namespace rematch
