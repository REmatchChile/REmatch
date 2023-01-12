#ifndef STRUCTS__ECS__ECS_HPP
#define STRUCTS__ECS__ECS_HPP

#include <bitset>
#include <sstream>
#include <string>
#include <vector>

#include "ecs/node_manager.hpp"
#include "ecs_node.hpp"

namespace rematch {

class ECS {
 public:
  size_t get_amount_of_nodes_used() const;
  size_t amount_of_nodes_allocated() const { return node_manager.amount_of_nodes_allocated(); }
 private:
  NodeManager node_manager; // node_manager

 public:
  ECS() = default;

  ECSNode *create_bottom_node(); // create_bottom_node
  ECSNode *create_extend_node(ECSNode* node, std::bitset<64> variable_markers,
                  int document_position); // create_extend_node
  ECSNode *create_union_node(ECSNode *node_1, ECSNode *node_2); // create_unite_node

  ECSNode *pin_node(ECSNode *node); // pin_node
  void unpin_node(ECSNode *node); // unpin_node

  private:
    ECSNode *create_union_of_two_non_output_nodes(
        ECSNode *node_1, ECSNode *node_2);
    ECSNode *create_first_intermediate_union_node(ECSNode *node_1,
                                                  ECSNode *node_2);
    ECSNode *create_second_intermediate_union_node(ECSNode *node_2,
                                                   ECSNode *u1);
    ECSNode *create_union_of_output_and_intermediate_node(
        ECSNode *node_1, ECSNode *u2);
}; // end class ECS

} // end namespace rematch

#endif
