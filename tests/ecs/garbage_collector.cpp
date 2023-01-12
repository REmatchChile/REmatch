#include <catch2/catch_test_macros.hpp>
#include <vector>
#include "ecs/garbage_collector.hpp"

//#include "regex/regex.hpp"
//#include "data/search.cpp"

namespace rematch::testing {

std::vector<ECSNode*> create_numbered_label_nodes_that_are_connected(
      GarbageCollector *garbage_collector,
      int amount_of_nodes_to_alloc, int starting_label
  ) {
  ECSNode *bottom_node = garbage_collector->alloc(ECSNodeType::kBottom);
  bottom_node->document_position = starting_label;
  std::vector<ECSNode*> nodes;
  nodes.push_back(bottom_node);
  ECSNode *last_node = bottom_node;
  for (int i = 1; i < amount_of_nodes_to_alloc; i++) {
    ECSNode *new_node = garbage_collector->alloc(
        ECSNodeType::kLabel, last_node, nullptr, 0, i + starting_label);
    nodes.push_back(new_node);
    REQUIRE(new_node->next() != nullptr);
    garbage_collector->decrease_ref_count(last_node);
    last_node = new_node;
  }
  return nodes;
}

TEST_CASE("Garbage collector doesn't recycle nodes that are being used") {
  auto *garbage_collector = new GarbageCollector();
  std::vector<ECSNode*> nodes = create_numbered_label_nodes_that_are_connected(
      garbage_collector, MEMORY_POOL_STARTING_SIZE, 0);
  for (int i = 1; i < (int) MEMORY_POOL_STARTING_SIZE; i++) {
    REQUIRE(nodes[i]->document_position == i);
  }
}

TEST_CASE("After liberating the tail of a linked list node with one reference to it, all previous nodes will be eventually liberated and reused. no new memory will be used and the order of the new nodes will be added from the tail to the head of the original nodes.") {
  auto *garbage_collector = new GarbageCollector();
  std::vector<ECSNode*> nodes = create_numbered_label_nodes_that_are_connected(
      garbage_collector, MEMORY_POOL_STARTING_SIZE, 0);
  garbage_collector->decrease_ref_count(nodes[MEMORY_POOL_STARTING_SIZE - 1]);

  create_numbered_label_nodes_that_are_connected(
      garbage_collector, MEMORY_POOL_STARTING_SIZE, MEMORY_POOL_STARTING_SIZE);

  INFO("Recycled nodes: " << garbage_collector->amount_of_recycled_nodes);
  INFO("Total nodes: " << garbage_collector->amount_of_nodes_used);

  for (int i = 0; i < (int) MEMORY_POOL_STARTING_SIZE; i++) {
    REQUIRE(nodes[MEMORY_POOL_STARTING_SIZE - i - 1]->document_position == i + MEMORY_POOL_STARTING_SIZE);
  }
}

size_t total_size_of_nth_minipool(int n) {
  size_t out = 0;
  size_t next_allocation = MEMORY_POOL_STARTING_SIZE;
  for (int i = 1; i <= n; i++) {
    out += next_allocation;
    next_allocation *= 2;
  }
  return out;
}

TEST_CASE("The duplication of memory is working correctly") {
  auto *garbage_collector = new GarbageCollector();

  REQUIRE(garbage_collector->amount_of_nodes_allocated() 
                      == 
          MEMORY_POOL_STARTING_SIZE
  );
  int amount_of_nodes_to_allocate = MEMORY_POOL_STARTING_SIZE;
  for (int memory_duplication_time = 1;
           memory_duplication_time < 5;
           memory_duplication_time++) {
    std::vector<ECSNode*> nodes = create_numbered_label_nodes_that_are_connected(
        garbage_collector, amount_of_nodes_to_allocate, 0);
    amount_of_nodes_to_allocate *= 2;

    REQUIRE(total_size_of_nth_minipool(memory_duplication_time) 
                        == 
            garbage_collector->amount_of_nodes_allocated()
    );
  }

}



}  // namespace rematch::testing
