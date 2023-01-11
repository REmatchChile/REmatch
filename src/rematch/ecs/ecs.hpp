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
  size_t n_nodes() const { return garbage_collector_.n_nodes(); }
  size_t tot_size() const { return garbage_collector_.tot_size(); }
 private:
  GarbageCollector garbage_collector_;

 public:
  ECS() = default;

  ECSNode* extend(ECSNode* v, std::bitset<32> S, int64_t i);
  ECSNode *unite(ECSNode *v1, ECSNode *v2, bool garbage_left=false);
  ECSNode *bottom_node() {return garbage_collector_.alloc(ECSNodeType::kBottom);}

  void try_mark_unused(ECSNode *v) {
    if (v->ref_count == 0)
      garbage_collector_.add_to_list_of_free_memory(v);
  }

  // Add function that accepts a ECSNode and manages the ref_count to this node.
  // liberate_node? return_node? copy_node and delete_nodetk

  private:
    ECSNode *create_union_of_two_non_output_nodes(
        ECSNode *v1, ECSNode *v2, bool garbage_left);
    ECSNode *create_first_intermediate_union_node(ECSNode *node_1,
                                                  ECSNode *node_2);
    ECSNode *create_second_intermediate_union_node(ECSNode *node_2,
                                                   ECSNode *u1);
    ECSNode *create_union_node(ECSNode *node_1, ECSNode *u2);
    void rectify_node_references_after_union(ECSNode *node_1, ECSNode *node_2);
    void check_if_previous_nodes_are_unused(ECSNode *node_1,
                                            ECSNode *node_2,
                                            bool garbage_left);



  

}; // end class ECS

} // end namespace rematch

#endif
