#include "output_enumeration/enumerator.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include "output_enumeration/ecs.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

TEST_CASE("Enumerator returns only one mapping when no unions are used") {
  ECS ecs = ECS();
  int depth = GENERATE(0, 3, 7, 31);
  ECSNode* linked_list = create_linked_list_node_of_depth(&ecs, depth);
  Enumerator enumerator = Enumerator();
  enumerator.add_node(linked_list);
  int amount_of_mappings = 0;
  while (enumerator.has_next()) {
    enumerator.next();
    amount_of_mappings++;
  }
  REQUIRE(amount_of_mappings == 1);
}

TEST_CASE("One union returns two mappings") {
  ECS ecs = ECS();
  int depth = GENERATE(0, 3, 7, 31);
  ECSNode* union_node =
      ecs.create_union_node(create_linked_list_node_of_depth(&ecs, depth),
                            create_linked_list_node_of_depth(&ecs, depth));
  Enumerator enumerator = Enumerator();
  enumerator.add_node(union_node);
  int amount_of_mappings = 0;
  while (enumerator.has_next()) {
    enumerator.next();
    amount_of_mappings++;
  }
  REQUIRE(amount_of_mappings == 2);
}

void add_oppening_annotation_to_variable(std::bitset<64>& variable_markers,
                                         int variable) {
  variable_markers[variable * 2] = true;
}

void add_closing_annotation_to_variable(std::bitset<64>& variable_markers,
                                        int variable) {
  variable_markers[variable * 2 + 1] = true;
}

ECSNode* extend_current_node_with_annotation(ECS* ecs, ECSNode* current_node,
                                             std::bitset<64> variable_markers,
                                             int& last_document_position) {
  ECSNode* new_left_node = ecs->create_extend_node(
      current_node, variable_markers, last_document_position--);
  ECSNode* new_right_node = ecs->create_extend_node(
      current_node, variable_markers, last_document_position--);
  ECSNode* new_union_node =
      ecs->create_union_node(new_left_node, new_right_node);
  return new_union_node;
}

ECSNode* extend_current_node_with_closing_hexagon(ECS* ecs,
                                                  ECSNode* current_node,
                                                  int variable_number,
                                                  int& last_document_position) {
  std::bitset<64> variable_markers;
  add_closing_annotation_to_variable(variable_markers, variable_number);
  return extend_current_node_with_annotation(
      ecs, current_node, variable_markers, last_document_position);
}

ECSNode* extend_current_node_with_opening_hexagon(ECS* ecs,
                                                  ECSNode* current_node,
                                                  int variable_number,
                                                  int& last_document_position) {
  std::bitset<64> variable_markers;
  add_oppening_annotation_to_variable(variable_markers, variable_number);
  return extend_current_node_with_annotation(
      ecs, current_node, variable_markers, last_document_position);
}

ECSNode* create_complex_ecs_structure_1(ECS* ecs, int amount_of_variables) {
  /*       _\|/_
           (o o)
   +----oOO-{_}-OOo----------------------------------+
   |#################################################|
   |#       Union                                   #|
   |#      /     \                                  #|
   |#     /       \                                 #|
   |# Extend      Extend                            #|
   |#     \       /                                 #|
   |#      \     /                                  #|
   |#       Union                                   #|
   |#         .       The first complex structure   #|
   |#         .       creates hexagons where the    #|
   |#         .       layers use different varia-   #|
   |#         .       bles that are opened and      #|
   |#       Union     closed.                       #|
   |#      /     \                                  #|
   |#     /       \                                 #|
   |# Extend      Extend                            #|
   |#     \       /                                 #|
   |#      \     /                                  #|
   |#       Bottom                                  #|
   |#################################################|
   +------------------------------------------------*/
  ECSNode* bottom = ecs->create_bottom_node();
  int last_document_position_used = 10000;
  ECSNode* current_node = bottom;
  for (int i = 0; i < amount_of_variables; i++)
    current_node = extend_current_node_with_closing_hexagon(
        ecs, current_node, i, last_document_position_used);
  for (int i = 0; i < amount_of_variables; i++)
    current_node = extend_current_node_with_opening_hexagon(
        ecs, current_node, i, last_document_position_used);
  return current_node;
}

TEST_CASE("Complex ECS structure returns correct amount of mappings") {
  ECS* ecs = new ECS();
  int amount_of_variables = GENERATE(1, 3, 5, 7);
  Enumerator enumerator = Enumerator();
  enumerator.add_node(create_complex_ecs_structure_1(ecs, amount_of_variables));

  int amount_of_mappings = 0;
  while (enumerator.has_next()) {
    enumerator.next();
    amount_of_mappings++;
  }
  REQUIRE(amount_of_mappings == pow(2, amount_of_variables * 2));
  delete ecs;
}

int get_height_of_tree(int amount_of_nodes) {
  int log2 = 0;
  while (amount_of_nodes > 0) {
    amount_of_nodes /= 2;
    log2++;
  }
  return log2;
}

ECSNode* create_single_variable_output_node_with_position(
    ECS* ecs, int document_position) {
  std::bitset<64> variable_markers;
  variable_markers[0] = true;
  variable_markers[1] = true;
  ECSNode* output = ecs->create_extend_node(
      ecs->create_bottom_node(), variable_markers, document_position);
  REQUIRE(output != nullptr);
  return output;
}

ECSNode* create_binary_tree_of_union_nodes_with_one_variable_and_empty_captures(
    ECS* ecs, int depth) {
  ECSNode** last_layer = new ECSNode*[1 << depth];
  for (int i = 0; i < 1 << depth; i++) {
    last_layer[i] = create_single_variable_output_node_with_position(ecs, i);
  }
  for (int layer_number = depth - 1; layer_number >= 0; layer_number--) {
    ECSNode** current_layer = new ECSNode*[1 << layer_number];
    for (int i = 0; i < (1 << layer_number); i++) {
      current_layer[i] =
          ecs->create_union_node(last_layer[i * 2], last_layer[i * 2 + 1]);
    }
    delete[] last_layer;
    last_layer = current_layer;
  }
  return last_layer[0];
}

TEST_CASE(
    "Complete Binary tree with single variable output nodes as leafs \
           return correct mappings: 1,...,2^depth") {
  ECS* ecs = new ECS();
  int depth = GENERATE(1, 3, 8);
  ECSNode* root =
      create_binary_tree_of_union_nodes_with_one_variable_and_empty_captures(
          ecs, depth);
  Enumerator enumerator = Enumerator();
  enumerator.add_node(root);

  std::vector<int> spans = std::vector<int>();
  while (enumerator.has_next()) {
    std::vector<Span> spans_matched =
        enumerator.next()->get_spans_of_variable_id(0);
    REQUIRE(spans_matched.size() == 1);
    REQUIRE(spans_matched[0].first == spans_matched[0].second);
    spans.push_back(spans_matched[0].first);
  }

  std::vector<int> expected_spans = std::vector<int>();
  for (int i = 0; i < 1 << depth; i++) {
    expected_spans.push_back(i);
  }

  REQUIRE_THAT(spans, Catch::Matchers::UnorderedEquals(expected_spans));
  delete ecs;
}

TEST_CASE("Enumerator returns correct mapping after calling reset") {
  ECS ecs = ECS();
  int depth = 5;
  ECSNode* union_node =
      ecs.create_union_node(create_linked_list_node_of_depth(&ecs, depth),
                            create_linked_list_node_of_depth(&ecs, depth));
  Enumerator enumerator = Enumerator();
  enumerator.add_node(union_node);
  while (enumerator.has_next()) {
    enumerator.next();
  }

  enumerator.reset();
  enumerator.add_node(union_node);

  int amount_of_mappings = 0;
  while (enumerator.has_next()) {
    enumerator.next();
    amount_of_mappings++;
  }

  REQUIRE(amount_of_mappings == 2);
}

}  // namespace REmatch::testing
