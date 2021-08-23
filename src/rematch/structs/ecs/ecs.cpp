#include "ecs.hpp"

namespace rematch {
namespace internal {


ECS::Node* ECS::extend(ECS::Node* v, std::bitset<32> S, int64_t i) {
  ECS::LabelNode* v_prim = new ECS::LabelNode(S, i);
  v_prim->next_ = v;
  v->ref_count_++;
  return v_prim;
}

ECS::Node* ECS::unite(ECS::Node* v1, ECS::Node* v2) {
  ECS::UnionNode* v_prim;
  if(v1->is_output()) {
    v_prim = new UnionNode(v1, v2);
    v1->ref_count_++; v2->ref_count_++;
  } else if (v2->is_output()) {
    v_prim = new UnionNode(v2, v1);
    v1->ref_count_++; v2->ref_count_++;
  } else {
    ECS::UnionNode* u2 = new ECS::UnionNode(v1->right(), v2->right());
    v1->right()->ref_count_++; v2->right()->ref_count_++;
    ECS::UnionNode* u1 = new ECS::UnionNode(v2->left(), u2);
    v2->left()->ref_count_++; u2->ref_count_++;
    v_prim = new UnionNode(v1->left(), u1);
    v1->left()->ref_count_++; u1->ref_count_++;
  }
  return v_prim;
}

void ECS::mark_unused(ECS::Node* v) {
  if(v == nullptr) return;
  // if(v->ref_count_ == 0 && !v->is_empty())
}

} // end namespace rematch
} // end namespace internal