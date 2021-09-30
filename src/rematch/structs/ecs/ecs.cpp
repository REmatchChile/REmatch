#include "ecs.hpp"

namespace rematch {
namespace internal {


uint32_t ECS::Node::NextID = 0;


ECS::Node::Node(NodeType t, Node* left, Node* right, std::bitset<32> S, int64_t i) {
  id_ = NextID++;
  switch(t) {
    case NodeType::kBottom:
      S_[S_.size()-2] = 1;
      break;
    case NodeType::kUnion:
      left_ = left;
      right_ = right;
      S_[S_.size()-1] = 1;
      break;
    case NodeType::kLabel:
      left_ = left;
      S_ = S;
      i_ = i;
      S_[S_.size()-1] = 1;
      S_[S_.size()-2] = 1;
  }
}

ECS::Node* ECS::Node::reset(NodeType t, Node* left, Node* right,
                            std::bitset<32> S, int64_t i) {

  S_ = S;
  ref_count_ = 0;
  left_ = nullptr;
  right_ = nullptr;
  ++reset_count_;

  switch(t) {
    case NodeType::kBottom:
      S_[S_.size()-2] = 1;
      break;
    case NodeType::kUnion:
      left_ = left;
      right_ = right;
      S_[S_.size()-1] = 1;
      break;
    case NodeType::kLabel:
      left_ = left;
      S_ = S;
      i_ = i;
      S_[S_.size()-1] = 1;
      S_[S_.size()-2] = 1;
  }

  return this;
}

ECS::Node* ECS::extend(ECS::Node* v, std::bitset<32> S, int64_t i) {
  ECS::Node* v_prim = pool_.alloc(NodeType::kLabel, v, nullptr, S, i);
  v->ref_count_++;
  return v_prim;
}

ECS::Node* ECS::unite(ECS::Node* v1, ECS::Node* v2) {
  ECS::Node* v_prim;
  if(v1->is_output()) {
    v_prim = pool_.alloc(NodeType::kUnion,v1, v2);
    v1->ref_count_++; v2->ref_count_++;
  } else if (v2->is_output()) {
    v_prim = pool_.alloc(NodeType::kUnion,v2, v1);
    v1->ref_count_++; v2->ref_count_++;
  } else {
    ECS::Node* u2 = pool_.alloc(NodeType::kUnion, v1->right(), v2->right());
    v1->right()->ref_count_++; v2->right()->ref_count_++;
    ECS::Node* u1 = pool_.alloc(NodeType::kUnion, v2->left(), u2);
    v2->left()->ref_count_++; u2->ref_count_++;
    v_prim = pool_.alloc(NodeType::kUnion, v1->left(), u1);
    v1->left()->ref_count_++; u1->ref_count_++;

    // Maybe v1 and v2 end up being useless
    try_mark_unused(v1);
    try_mark_unused(v2);
  }
  return v_prim;
}

void ECS::try_mark_unused(ECS::Node* v) {
  if(v->ref_count_ == 0)
    pool_.add_to_free_list(v);
}

} // end namespace rematch
} // end namespace internal