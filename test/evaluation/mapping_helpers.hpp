#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include "dummy_mapping.hpp"

namespace REmatch::testing {

template <typename T>
bool contains_mapping(std::vector<T>& mappings, T& target_mapping) {
  return std::find(mappings.begin(), mappings.end(), target_mapping) != mappings.end();
}
template <typename T>
void remove_mapping_from_expected(std::vector<T>& mappings, T& target_mapping) {
  mappings.erase(std::remove(mappings.begin(), mappings.end(), target_mapping), mappings.end());
}

}
