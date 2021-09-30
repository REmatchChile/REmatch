#include <iostream>

#include "structs/ecs/ecs.hpp"

using namespace rematch::internal;

int main() {
  ECS ds;

  ECS::Node* n = ds.bottom_node();

  ECS::Node* n1 = ds.extend(n, std::bitset<32>("10000000000000000000000000000000"),0);
  ECS::Node* n2 = ds.extend(n1, std::bitset<32>("10100000000000000000000000000000"),1);
  ECS::Node* n3 = ds.extend(n, std::bitset<32>("10000000000000000000000000000000"),3);

  ECS::Node* n4 = ds.unite(n2, n3);
  ECS::Node* n5 = ds.unite(n1, n);
  ECS::Node* n6 = ds.unite(n4, n5);

  // std::cout << n6->print();

  return 0;
}