#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "output_enumeration/ecs_node.hpp"

namespace REmatch::testing {

ECSNode* dummynode = new ECSNode(ECSNodeType::kBottom);

TEST_CASE("is_bottom returns true with bottom nodes") {
  ECSNode *node = new ECSNode(ECSNodeType::kBottom);
  REQUIRE(node->is_bottom());
}

TEST_CASE("is_bottom does not return true with non bottom nodes") {
  for (ECSNodeType type : {ECSNodeType::kUnion,
                           ECSNodeType::kLabel}) {
    ECSNode *node = new ECSNode(type, dummynode, dummynode);
    REQUIRE(!node->is_bottom());
  }
}

void ensure_nodes_variable_markers_has_the_correct_value(
    ECSNode *node,
    int value) {
  node->variable_markers[node->variable_markers.size() - 1] = 0;
  node->variable_markers[node->variable_markers.size() - 2] = 0;
  REQUIRE(node->variable_markers == value);
}

TEST_CASE("label stores variable_markers correctly after removing the labels") {
  int value = GENERATE(0, 3, 7, 12, 500, 1000);
  ECSNode *node = new ECSNode(ECSNodeType::kLabel, dummynode, dummynode, value, 0);
  ensure_nodes_variable_markers_has_the_correct_value(node, value);
}

TEST_CASE("Union stores left and right nodes correctly") {
  int left_value = GENERATE(0, 3);
  int right_value = GENERATE(6, 9);

  ECSNode *left_node = new ECSNode(
      ECSNodeType::kLabel, dummynode, dummynode, left_value);
  ECSNode *right_node = new ECSNode(
      ECSNodeType::kLabel, dummynode, dummynode, right_value);

  ECSNode *union_node = new ECSNode(
      ECSNodeType::kUnion, left_node, right_node);

  ensure_nodes_variable_markers_has_the_correct_value(
      union_node->left_node(), left_value);
  ensure_nodes_variable_markers_has_the_correct_value(
      union_node->right_node(), right_value);
}

TEST_CASE("left_node and right_node of a bottom are nullptr's") {
  ECSNode *aux_node = new ECSNode(ECSNodeType::kBottom);
  ECSNode *node = new ECSNode(ECSNodeType::kBottom, aux_node, aux_node);
  REQUIRE(node->left_node() == nullptr);
  REQUIRE(node->right_node() == nullptr);
}

TEST_CASE("right_node of a label is a nullptr") {
  ECSNode *aux_node = new ECSNode(ECSNodeType::kBottom);
  ECSNode *node = new ECSNode(ECSNodeType::kLabel, aux_node, aux_node, 1);
  REQUIRE(node->right_node() == nullptr);
}

TEST_CASE("left_node of a label is the correct node") {
  ECSNode *aux_node = new ECSNode(ECSNodeType::kBottom);
  ECSNode *node = new ECSNode(ECSNodeType::kLabel, aux_node, aux_node, 1);
  REQUIRE(node->left_node() == aux_node);
}


}  // namespace REmatch::testing
