#include <iostream>

#include "structs/ecs/ecs.hpp"


int main() {
  rematch::ECS ds;

  rematch::ECS::Node *n = ds.bottom_node();

  rematch::ECS::Node *n1 =
      ds.extend(n, std::bitset<32>("00000000000000000000000000000001"), 0);
  rematch::ECS::Node *n2 =
      ds.extend(n1, std::bitset<32>("00000000000000000000000000000101"), 1);
  rematch::ECS::Node *n3 =
      ds.extend(n, std::bitset<32>("00000000000000000000000000000001"), 3);

  rematch::ECS::Node *n4 = ds.unite(n2, n3);
  rematch::ECS::Node *n5 = ds.unite(n1, n);
  rematch::ECS::Node *n8 = ds.unite(n4, n5);

  ds.try_mark_unused(n8);

  rematch::ECS::Node *n9 =
      ds.extend(n, std::bitset<32>("00000000000000000000000000000001"), 8);
  rematch::ECS::Node *n10 =
      ds.extend(n9, std::bitset<32>("00000000000000000000000000000011"), 10);
  rematch::ECS::Node *n11 =
      ds.extend(n10, std::bitset<32>("00000000000000000000000000000011"), 11);

  return 0;
}