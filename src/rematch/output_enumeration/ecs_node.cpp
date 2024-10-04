#include "output_enumeration/ecs_node.hpp"

namespace REmatch {
inline namespace output_enumeration {

ECSNode::ECSNode(ECSNodeType node_type, ECSNode *left,
                 ECSNode *right, std::bitset<64> variable_markers,
                 int document_position) {
  assign_attributes(node_type, left, right, variable_markers, document_position);
}

ECSNode *ECSNode::reset(ECSNodeType node_type, ECSNode *left, ECSNode *right,
                        std::bitset<64> variable_markers,
                        int document_position) {
  reset_attributes();
  assign_attributes(node_type, left, right, variable_markers, document_position);
  return this;
}

bool ECSNode::is_output() const {
  return variable_markers[variable_markers.size() - 2];
}

bool ECSNode::is_bottom() const {
  return variable_markers[variable_markers.size() - 2] &&
         !variable_markers[variable_markers.size() - 1];
}

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
    std::bitset<64> variable_markers, int document_position) {
  switch (node_type) {
  case ECSNodeType::kBottom:
    label_node_as_kBottom();
    break;
  case ECSNodeType::kUnion:
    create_kUnion_node(left, right);
    break;
  case ECSNodeType::kLabel:
    create_kLabel_node(left, variable_markers, document_position);
  }
}

void ECSNode::reset_attributes() {
  variable_markers = 0;
  ref_count = 0;
  left = nullptr;
  right = nullptr;
}

void ECSNode::label_node_as_kBottom() {
    variable_markers[variable_markers.size() - 2] = 1;
    type = ECSNodeType::kBottom;
}

void ECSNode::create_kUnion_node(ECSNode *left, ECSNode *right) {
    add_left_node(left);
    add_right_node(right);
    label_node_as_kUnion();
}

void ECSNode::add_left_node(ECSNode *node) {
  this->left = node;
  node->increase_ref_count();
}

void ECSNode::add_right_node(ECSNode *node) {
  this->right = node;
  node->increase_ref_count();
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
                                 std::bitset<64> variable_markers,
                                 int document_position) {
    add_left_node(left);
    this->variable_markers = variable_markers;
    label_node_as_kLabel();
    this->document_position = document_position;
}

}
}
