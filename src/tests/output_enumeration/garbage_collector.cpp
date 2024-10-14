#include <catch2/catch_test_macros.hpp>
#include <REmatch/REmatch.hpp>
#include <vector>
#include "output_enumeration/node_manager.hpp"

//#include "regex/regex.hpp"
//#include "data/search.cpp"

namespace REmatch::testing {

std::vector<ECSNode*> create_numbered_label_nodes_that_are_connected(
      NodeManager *node_manager,
      int amount_of_nodes_to_alloc, int starting_label
  ) {
  ECSNode *bottom_node = node_manager->alloc(ECSNodeType::kBottom);
  bottom_node->document_position = starting_label;
  node_manager->increase_ref_count(bottom_node);
  std::vector<ECSNode*> nodes;
  nodes.push_back(bottom_node);
  ECSNode *last_node = bottom_node;
  for (int i = 1; i < amount_of_nodes_to_alloc; i++) {
    ECSNode *new_node = node_manager->alloc(
        ECSNodeType::kLabel, last_node, nullptr, 0, i + starting_label);
    node_manager->increase_ref_count(new_node);
    nodes.push_back(new_node);
    REQUIRE(new_node->next() != nullptr);
    node_manager->decrease_ref_count(last_node);
    last_node = new_node;
  }
  return nodes;
}

TEST_CASE("Garbage collector doesn't recycle nodes that are being used") {
  auto *node_manager = new NodeManager(REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  std::vector<ECSNode*> nodes = create_numbered_label_nodes_that_are_connected(
      node_manager, MEMORY_POOL_STARTING_SIZE, 0);
  for (int i = 1; i < (int) MEMORY_POOL_STARTING_SIZE; i++) {
    REQUIRE(nodes[i]->document_position == i);
  }
}

TEST_CASE(
    "After liberating the tail of a linked list node with one \
    reference to it, all previous nodes will be eventually liberated \
    and reused. no new memory will be used and the order of the new \
    nodes added will be from the tail to the head of the original \
    nodes.") {
  auto *node_manager = new NodeManager(REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  std::vector<ECSNode*> original_nodes =
    create_numbered_label_nodes_that_are_connected(node_manager,
                                                   MEMORY_POOL_STARTING_SIZE, 0);
  node_manager->decrease_ref_count(
      original_nodes[MEMORY_POOL_STARTING_SIZE - 1]);

  create_numbered_label_nodes_that_are_connected(
      node_manager, MEMORY_POOL_STARTING_SIZE, MEMORY_POOL_STARTING_SIZE);

  INFO("Recycled nodes: " << node_manager->amount_of_recycled_nodes);
  INFO("Total nodes: " << node_manager->amount_of_nodes_used);

  for (int i = 0; i < (int) MEMORY_POOL_STARTING_SIZE; i++) {
    REQUIRE(
        (size_t) original_nodes[MEMORY_POOL_STARTING_SIZE - i - 1]->document_position
                                      ==
                          i + MEMORY_POOL_STARTING_SIZE
    );
  }
}

size_t amount_of_nodes_that_n_minipool_should_allocate(int n) {
  size_t out = 0;
  size_t next_allocation = MEMORY_POOL_STARTING_SIZE;
  for (int i = 1; i <= n; i++) {
    out += next_allocation;
    next_allocation *= 2;
  }
  return out;
}

TEST_CASE("The duplication of memory is working correctly") {
  auto *node_manager = new NodeManager(4);

  REQUIRE(node_manager->amount_of_nodes_allocated()
                      ==
          MEMORY_POOL_STARTING_SIZE
  );
  int amount_of_nodes_to_allocate = MEMORY_POOL_STARTING_SIZE;
  for (int memory_duplication_time = 1;
           memory_duplication_time < 5;
           memory_duplication_time++) {
    std::vector<ECSNode*> nodes = create_numbered_label_nodes_that_are_connected(
        node_manager, amount_of_nodes_to_allocate, 0);
    amount_of_nodes_to_allocate *= 2;

    REQUIRE(
        amount_of_nodes_that_n_minipool_should_allocate(memory_duplication_time)
                                       ==
                  node_manager->amount_of_nodes_allocated()
    );
  }
}



}  // namespace REmatch::testing
