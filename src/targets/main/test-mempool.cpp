#include <iostream>

#include "structs/ecs/ecs.hpp"

using namespace rematch::internal;

int main() {
  ECS ds;

  ECS::Node* n = ds.bottom_node();

  ECS::Node* n1 = ds.extend(n, std::bitset<32>("00000000000000000000000000000001"),0);
  ECS::Node* n2 = ds.extend(n1, std::bitset<32>("00000000000000000000000000000101"),1);
  ECS::Node* n3 = ds.extend(n, std::bitset<32>("00000000000000000000000000000001"),3);

  ECS::Node* n4 = ds.unite(n2, n3);
  ECS::Node* n5 = ds.unite(n1, n);
  ECS::Node* n8 = ds.unite(n4, n5);

  ds.try_mark_unused(n8);

  ECS::Node* n9 = ds.extend(n, std::bitset<32>("00000000000000000000000000000001"),8);
  ECS::Node* n10 = ds.extend(n9, std::bitset<32>("00000000000000000000000000000011"),10);
  ECS::Node* n11 = ds.extend(n10, std::bitset<32>("00000000000000000000000000000011"),11);

  return 0;
}