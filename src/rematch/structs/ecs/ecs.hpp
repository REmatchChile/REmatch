#ifndef STRUCTS__ECS__ECS_HPP
#define STRUCTS__ECS__ECS_HPP

#include <bitset>
#include <sstream>
#include <string>
#include <vector>

#include "structs/ecs/garbage_collector.hpp"
#include "ecs_node.hpp"

namespace rematch {

class ECS {
 public:



  ECS() = default;

  ECSNode* extend(ECSNode* v, std::bitset<32> S, int64_t i);

  ECSNode *unite(ECSNode *v1, ECSNode *v2, bool delay=false);

  ECSNode *bottom_node() { return garbage_collector_.alloc(ECSNodeType::kBottom); }

  // Marks the node as unused iff its ref_count == 0. Otherwise nothing
  // happens. The Node MUST be present in this->garbage_collector_.
  void try_mark_unused(ECSNode *v) {
    if (v->ref_count == 0)
      garbage_collector_.add_to_list_of_free_memory(v);
  }

  // Add function that accepts a ECSNode and manages the ref_count to this node.
  // liberate_node? return_node? copy_node and delete_nodetk

  size_t n_nodes() const { return garbage_collector_.n_nodes(); }
  size_t tot_size() const { return garbage_collector_.tot_size(); }

 private:
  GarbageCollector garbage_collector_;
  

}; // end class ECS

} // end namespace rematch

#endif
