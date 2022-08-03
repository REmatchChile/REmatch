#include <iostream>
#include <memory>

#include "ecs.hpp"

namespace rematch {
uint32_t ECS::Node::NextID = 0;

ECS::Node::Node(NodeType t, Node *left, Node *right, std::bitset<32> S,
                int64_t i) {
  id_ = NextID++;

  switch (t) {
  case NodeType::kBottom:
    S_[S_.size() - 2] = 1;
    type_ = NodeType::kBottom;
    break;
  case NodeType::kUnion:
    left_ = left;
    right_ = right;
    S_[S_.size() - 1] = 1;
    type_ = NodeType::kUnion;
    break;
  case NodeType::kLabel:
    left_ = left;
    S_ = S;
    i_ = i;
    S_[S_.size() - 1] = 1;
    S_[S_.size() - 2] = 1;
    type_ = NodeType::kLabel;
  }
}

ECS::Node *ECS::Node::reset(NodeType t, Node *left, Node *right,
                            std::bitset<32> S, int64_t i) {
  S_ = S;
  ref_count_ = 1;
  left_ = nullptr;
  right_ = nullptr;

  ++reset_count_;

  switch (t) {
  case NodeType::kBottom:
    S_[S_.size() - 2] = 1;
    type_ = NodeType::kBottom;
    break;
  case NodeType::kUnion:
    left_ = left;
    right_ = right;
    S_[S_.size() - 1] = 1;
    type_ = NodeType::kUnion;
    break;
  case NodeType::kLabel:
    left_ = left;
    S_ = S;
    i_ = i;
    S_[S_.size() - 1] = 1;
    S_[S_.size() - 2] = 1;
    type_ = NodeType::kLabel;
  }

  return this;
}

ECS::Node *ECS::extend(ECS::Node *v, std::bitset<32> S, int64_t i) {
  ECS::Node *v_prim = pool_.alloc(NodeType::kLabel, v, nullptr, S, i);
  return v_prim;
}

ECS::Node *ECS::unite(ECS::Node *v1, ECS::Node *v2, bool garbage_left) {
  ECS::Node *v_prim;
  if (v1->is_output()) {
    v_prim = pool_.alloc(NodeType::kUnion, v1, v2);
  } else if (v2->is_output()) {
    v_prim = pool_.alloc(NodeType::kUnion, v2, v1);
  } else {
    ECS::Node *u2 = pool_.alloc(NodeType::kUnion, v1->right(), v2->right());
    v1->right()->inc_ref_count();
    v2->right()->inc_ref_count();
    ECS::Node *u1 = pool_.alloc(NodeType::kUnion, v2->left(), u2);
    v2->left()->inc_ref_count();
    v_prim = pool_.alloc(NodeType::kUnion, v1->left(), u1);
    v1->left()->inc_ref_count();

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