#include <catch2/catch_test_macros.hpp>
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/enumerator.hpp"
#include <catch2/generators/catch_generators.hpp>

namespace rematch::testing {

ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth) {
  if (depth == 0) {
    return ecs->create_bottom_node();
  } else {
    return ecs->create_extend_node(
        create_linked_list_node_of_depth(ecs, depth - 1),
        depth,
        depth
    );
  }
}

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
  ECSNode* union_node = ecs.create_union_node(
      create_linked_list_node_of_depth(&ecs, depth),
      create_linked_list_node_of_depth(&ecs, depth)
  );
  Enumerator enumerator = Enumerator();
  enumerator.add_node(union_node);
  int amount_of_mappings = 0;
  while (enumerator.has_next()) {
    enumerator.next();
    amount_of_mappings++;
  }
  REQUIRE(amount_of_mappings == 2);
}

void add_oppening_annotation_to_variable(
    std::bitset<64> &variable_markers, int variable
) {
  variable_markers[variable * 2] = true;
}

void add_closing_annotation_to_variable(
    std::bitset<64> &variable_markers, int variable
) {
  variable_markers[variable * 2 + 1] = true;
}

ECSNode *extend_current_node_with_annotation(
    ECS *ecs, ECSNode *current_node, std::bitset<64> variable_markers,
    int &last_document_position) {
  ECSNode *new_left_node = ecs->create_extend_node(
      current_node,
      variable_markers,
      last_document_position--
  );
  ECSNode *new_right_node = ecs->create_extend_node(
      current_node,
      variable_markers,
      last_document_position--
  );
  ecs->unpin_node(current_node);
  ECSNode *new_union_node = ecs->create_union_node(
      new_left_node, new_right_node
  );
  ecs->unpin_node(new_left_node);
  ecs->unpin_node(new_right_node);
  return new_union_node;
}

ECSNode *extend_current_node_with_closing_hexagon(
    ECS *ecs, ECSNode *current_node, int variable_number,
    int &last_document_position) {
  std::bitset<64> variable_markers;
  add_closing_annotation_to_variable(variable_markers, variable_number);
  return extend_current_node_with_annotation(
      ecs, current_node, variable_markers, last_document_position
  );
}

ECSNode *extend_current_node_with_opening_hexagon(
    ECS *ecs, ECSNode *current_node, int variable_number,
    int &last_document_position) {
  std::bitset<64> variable_markers;
  add_oppening_annotation_to_variable(variable_markers, variable_number);
  return extend_current_node_with_annotation(
      ecs, current_node, variable_markers, last_document_position
  );
}

TEST_CASE("Complex ECS structure returns correct mappings") {
  /*       _\|/_
           (o o)
   +----oOO-{_}-OOo----------------------------------+
   |#################################################|
   |#       Union                                   #|
   |#      /     \                                  #|
   |#     /       \                                 #|
   |# Extend      Extend                            #|
   |#     \       /                                 #|
   |#      \     /          The test case will be a #|
   |#       Union           ECS created in the form #|
   |#         .             that is shown in the    #|
   |#         .             left. Note that this    #|
   |#         .             test will use different #|
   |#         .             variables for each      #|
   |#       Union           layer of extends, thus  #|
   |#      /     \          being functional.       #|
   |#     /       \                                 #|
   |# Extend      Extend                            #|
   |#     \       /                                 #|
   |#      \     /                                  #|
   |#       Bottom                                  #|
   |#################################################|
   +------------------------------------------------*/
  ECS *ecs = new ECS();
  ECSNode* bottom = ecs->create_bottom_node();
  int last_document_position_used = 10000;
  int amount_of_variables = GENERATE(1, 3, 5, 7);
  ECSNode* current_node = bottom;
  for (int i = 0; i < amount_of_variables; i++)
    current_node = extend_current_node_with_closing_hexagon(
        ecs, current_node, i, last_document_position_used
    );
  for (int i = 0; i < amount_of_variables; i++)
    current_node = extend_current_node_with_opening_hexagon(
        ecs, current_node, i, last_document_position_used
    );
  Enumerator enumerator = Enumerator(amount_of_variables);
  enumerator.add_node(current_node);
  int amount_of_mappings = 0;
  while (enumerator.has_next()) {
    enumerator.next();
    amount_of_mappings++;
  }
  REQUIRE(amount_of_mappings == pow(2, amount_of_variables * 2));
  delete ecs;
}


}  // namespace rematch::testing
