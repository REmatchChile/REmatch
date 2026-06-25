#include "output_enumeration/ecs_node.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace REmatch::testing {

TEST_CASE("is_bottom returns true with bottom nodes") {
  auto node = ECSNode(ECSNodeType::kBottom);
  REQUIRE(node.is_bottom());
}

TEST_CASE("is_bottom does not return true with non bottom nodes") {
  auto bottom = ECSNode(ECSNodeType::kBottom);
  for (ECSNodeType type : {ECSNodeType::kUnion, ECSNodeType::kLabel}) {
    auto node = ECSNode(type, &bottom, &bottom);
    REQUIRE(!node.is_bottom());
  }
}

void ensure_nodes_variable_markers_has_the_correct_value(ECSNode* node, int value) {
  node->variable_markers[node->variable_markers.size() - 1] = false;
  node->variable_markers[node->variable_markers.size() - 2] = false;
  REQUIRE(node->variable_markers == value);
}

TEST_CASE("label stores variable_markers correctly after removing the labels") {
  auto bottom = ECSNode(ECSNodeType::kBottom);
  int value = GENERATE(0, 3, 7, 12, 500, 1000);
  auto node = ECSNode(ECSNodeType::kLabel, &bottom, &bottom, value, 0);
  ensure_nodes_variable_markers_has_the_correct_value(&node, value);
}

TEST_CASE("Union stores left and right nodes correctly") {
  auto bottom = ECSNode(ECSNodeType::kBottom);
  int left_value = GENERATE(0, 3);
  int right_value = GENERATE(6, 9);

  auto left_node = ECSNode(ECSNodeType::kLabel, &bottom, &bottom, left_value);
  auto right_node = ECSNode(ECSNodeType::kLabel, &bottom, &bottom, right_value);
  auto union_node = ECSNode(ECSNodeType::kUnion, &left_node, &right_node);

  ensure_nodes_variable_markers_has_the_correct_value(union_node.left_node(), left_value);
  ensure_nodes_variable_markers_has_the_correct_value(union_node.right_node(), right_value);
}

TEST_CASE("left_node and right_node of a bottom are nullptr's") {
  auto aux_node = ECSNode(ECSNodeType::kBottom);
  auto node = ECSNode(ECSNodeType::kBottom, &aux_node, &aux_node);
  REQUIRE(node.left_node() == nullptr);
  REQUIRE(node.right_node() == nullptr);
}

TEST_CASE("right_node of a label is a nullptr") {
  auto aux_node = ECSNode(ECSNodeType::kBottom);
  auto node = ECSNode(ECSNodeType::kLabel, &aux_node, &aux_node, 1);
  REQUIRE(node.right_node() == nullptr);
}

TEST_CASE("left_node of a label is the correct node") {
  auto aux_node = ECSNode(ECSNodeType::kBottom);
  auto node = ECSNode(ECSNodeType::kLabel, &aux_node, &aux_node, 1);
  REQUIRE(node.left_node() == &aux_node);
}

}  // namespace REmatch::testing
