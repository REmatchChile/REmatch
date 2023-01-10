#include <iostream>
#include <memory>

#include "ecs_node.hpp"

namespace rematch {

ECSNode::ECSNode(ECSNodeType node_type, ECSNode *left,
                 ECSNode *right, std::bitset<32> variable_markers) {
  assign_attributes(node_type, left, right, variable_markers);
}

ECSNode *ECSNode::reset(ECSNodeType node_type, ECSNode *left, ECSNode *right,
                        std::bitset<32> variable_markers) {
  reset_attributes();
  ++reset_count_;
  assign_attributes(node_type, left, right, variable_markers);
  return this;
}

bool ECSNode::is_output() const {
  return variable_markers_[variable_markers_.size() - 2]; }

bool ECSNode::is_bottom() const {
  return variable_markers_[variable_markers_.size() - 2] && 
         !variable_markers_[variable_markers_.size() - 1]; }

void ECSNode::assign_attributes(
    ECSNodeType node_type, ECSNode *left, ECSNode *right,
    std::bitset<32> variable_markers) {
  switch (node_type) {
  case ECSNodeType::kBottom:
    label_node_as_kBottom();
    break;
  case ECSNodeType::kUnion:
    create_kUnion_node(left, right);
    break;
  case ECSNodeType::kLabel:
    create_kLabel_node(left, variable_markers);
  }
}

void ECSNode::reset_attributes() {
  variable_markers_ = 0;
  ref_count_ = 1;
  left_ = nullptr;
  right_ = nullptr;
}

void ECSNode::label_node_as_kBottom() {
    variable_markers_[variable_markers_.size() - 2] = 1;
    type_ = ECSNodeType::kBottom;
}

void ECSNode::create_kUnion_node(ECSNode *left, ECSNode *right) {
    left_ = left;
    right_ = right;
    label_node_as_kUnion();
}

void ECSNode::label_node_as_kUnion() {
    type_ = ECSNodeType::kUnion;
    variable_markers_[variable_markers_.size() - 1] = 1;
}

void ECSNode::label_node_as_kLabel() {
    variable_markers_[variable_markers_.size() - 1] = 1;
    variable_markers_[variable_markers_.size() - 2] = 1;
    type_ = ECSNodeType::kLabel;
}

void ECSNode::create_kLabel_node(ECSNode *left,
                                 std::bitset<32> variable_markers) {
    left_ = left;
    variable_markers_ = variable_markers;
    label_node_as_kLabel();
}

} // namespace rematch
