#ifndef OUTPUT_ENUMERATION__ECS_HPP
#define OUTPUT_ENUMERATION__ECS_HPP

#include <bitset>
#include <string>
#include <vector>

#include "output_enumeration/node_manager.hpp"
#include "output_enumeration/ecs_node.hpp"

#include <REmatch/constants.hpp>

namespace REmatch {

inline namespace output_enumeration {
using namespace REmatch;

  /**
   * An Enumerable Compact Set stores all combinations of outputs through
   * the interface of creating bottom nodes, extend nodes and union nodes
   * such that outputs can be enumerated with constant delay. Furthermore,
   * memory management is optimized through a node manager, so that memory
   * space is recycled and it is not necessary to allocate  memory an
   * extensive amount of times.
   *
   * This structure is specified by the paper:
   *   REmatch: a regex engine for finding all matches
   *   (Riveros, Van Sint, Vrgoc 2023).
   */
class ECS { // Enumerable Compact Set
 public:
  size_t get_amount_of_nodes_used() const;
  size_t amount_of_nodes_allocated() const { return node_manager.amount_of_nodes_allocated(); }
  size_t get_amount_of_nodes_reused() const;
 private:
  NodeManager node_manager;

 public:
  explicit ECS(uint_fast32_t max_mempool_duplications = REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  ~ECS() = default;

  /**
   * The bottom node, also known as the terminal node, has no children and
   * tells us that we reached the end of an output
   */
  ECSNode *create_bottom_node();

  /**
   * Extend nodes, also known as content nodes store the opened and closed
   * variables and the position in the document that this annotation is
   * referring to.
   */
  ECSNode *create_extend_node(ECSNode* node, std::bitset<64> variable_markers,
                  int document_position);
  /**
   * Union nodes allow us to enumerate multiple outputs starting from a
   * single node.
   */
  ECSNode *create_union_node(ECSNode *node_1, ECSNode *node_2);

  /**
   * If the node is going to be used in another scope, it is necessary to
   * pin it so that the memory manager does not recycle its memory location.
   */
  ECSNode *pin_node(ECSNode *node);

  /**
   * If a node is no longer used, it should be unpinned, this allows the
   * memory manager to recycle the memory allocated to it.
   */
  void unpin_node(ECSNode *node);

  private:
    ECSNode *create_union_of_two_non_output_nodes(
        ECSNode *node_1, ECSNode *node_2);
    ECSNode *create_first_intermediate_union_node(ECSNode *node_1,
                                                  ECSNode *node_2);
    ECSNode *create_second_intermediate_union_node(ECSNode *node_2,
                                                   ECSNode *u1);
    ECSNode *create_union_of_output_and_intermediate_node(
        ECSNode *node_1, ECSNode *u2);

};
}
}

#endif
