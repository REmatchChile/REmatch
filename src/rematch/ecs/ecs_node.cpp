#include <iostream>
#include <memory>

#include "ecs_node.hpp"

namespace rematch {

ECSNode::ECSNode(ECSNodeType node_type, ECSNode *left,
                 ECSNode *right, std::bitset<64> variable_markers) {
  assign_attributes(node_type, left, right, variable_markers);
}

ECSNode *ECSNode::reset(ECSNodeType node_type, ECSNode *left, ECSNode *right,
                        std::bitset<64> variable_markers) {
  reset_attributes();
  ++reset_count;
  assign_attributes(node_type, left, right, variable_markers);
  return this;
}

bool ECSNode::is_output() const {
  return variable_markers[variable_markers.size() - 2]; }

bool ECSNode::is_bottom() const {
  return variable_markers[variable_markers.size() - 2] && 
         !variable_markers[variable_markers.size() - 1]; }

std::ostream& operator<<(std::ostream& os, const ECSNode& n) {
  switch (n.type) {
    case ECSNodeType::kBottom:
      os << "bottom";
      break;
    case ECSNodeType::kUnion:
      os << "kUnion";
      break;
    case ECSNodeType::kLabel:
      os << "kLabel: " << n.variable_markers;
  }
  return os;
}

void ECSNode::assign_attributes(
    ECSNodeType node_type, ECSNode *left, ECSNode *right,
    std::bitset<64> variable_markers) {
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
  variable_markers = 0;
  ref_count = 1;
  left = nullptr;
  right = nullptr;
}

void ECSNode::label_node_as_kBottom() {
    variable_markers[variable_markers.size() - 2] = 1;
    type = ECSNodeType::kBottom;
}

void ECSNode::create_kUnion_node(ECSNode *left, ECSNode *right) {
    this->left = left;
    this->right = right;
    label_node_as_kUnion();
}

void ECSNode::label_node_as_kUnion() {
    type = ECSNodeType::kUnion;
    variable_markers[variable_markers.size() - 1] = 1;
}

void ECSNode::label_node_as_kLabel() {
    variable_markers[variable_markers.size() - 1] = 1;
    variable_markers[variable_markers.size() - 2] = 1;
    type = ECSNodeType::kLabel;
}

void ECSNode::create_kLabel_node(ECSNode *left,
                                 std::bitset<64> variable_markers) {
    this->left = left;
    this->variable_markers = variable_markers;
    label_node_as_kLabel();
}

} // namespace rematch
