#ifndef STRUCTS__ECS__ECS_HPP
#define STRUCTS__ECS__ECS_HPP

#include <bitset>
#include <sstream>
#include <string>
#include <vector>

#include "ecs/garbage_collector.hpp"
#include "ecs_node.hpp"

namespace rematch {

class ECS {
 public:
  size_t get_amount_of_nodes_used() const;
  size_t amount_of_nodes_allocated() const { return garbage_collector.amount_of_nodes_allocated(); }
 private:
  GarbageCollector garbage_collector;

 public:
  ECS() = default;

  ECSNode *bottom_node();
  ECSNode* extend(ECSNode* node, std::bitset<64> variable_markers,
                  int document_position);
  ECSNode *unite(ECSNode *node_1, ECSNode *node_2);

  ECSNode *copy_node(ECSNode *node);
  void return_node(ECSNode *node);

  private:
    ECSNode *create_union_of_two_non_output_nodes(
        ECSNode *node_1, ECSNode *node_2);
    ECSNode *create_first_intermediate_union_node(ECSNode *node_1,
                                                  ECSNode *node_2);
    ECSNode *create_second_intermediate_union_node(ECSNode *node_2,
                                                   ECSNode *u1);
    ECSNode *create_union_node(ECSNode *node_1, ECSNode *u2);
}; // end class ECS

} // end namespace rematch

#endif
