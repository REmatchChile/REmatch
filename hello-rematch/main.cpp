#include <iostream>
#include <REmatch/REmatch.hpp>

int main() {
  auto query = REmatch::reql("!x{that}");
  auto match_generator = query.finditer("thathathat");

  for (auto& match : match_generator) {
    std::cout << match << "\n";
  }

  return 0;
}