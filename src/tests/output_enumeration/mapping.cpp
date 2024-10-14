#include "output_enumeration/mapping.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

namespace REmatch::testing {

void add_annotations(Mapping& mapping,
                     std::vector<std::pair<std::bitset<64>, int>> annotations) {
  for (auto annotation : annotations) {
    mapping.add_annotations(annotation.first, annotation.second);
  }
}

TEST_CASE("Mapping without annotations results in an empty map") {
  auto mapping = Mapping();

  std::map<int, std::vector<Span>> mappings_map = mapping.construct_mapping();
  REQUIRE(mappings_map.size() == 0);
}

TEST_CASE("Mapping with a single variable returns the correct map") {
  auto mapping = Mapping();
  add_annotations(mapping,
                  {{std::bitset<64>("10"), 2}, {std::bitset<64>("01"), 0}});

  std::map<int, std::vector<Span>> expected_map = {
      {0, {Span{0, 2}}},
  };

  std::map<int, std::vector<Span>> mappings_map = mapping.construct_mapping();
  REQUIRE(mappings_map == expected_map);
}

TEST_CASE("Mapping with multiple variables returns the correct map") {
  auto mapping = Mapping();
  add_annotations(mapping, {{std::bitset<64>("0010"), 2},
                            {std::bitset<64>("1000"), 1},
                            {std::bitset<64>("0101"), 0}});

  std::map<int, std::vector<Span>> expected_map = {{0, {Span{0, 2}}},
                                                   {1, {Span{0, 1}}}};

  std::map<int, std::vector<Span>> mappings_map = mapping.construct_mapping();
  REQUIRE(mappings_map == expected_map);
}

TEST_CASE(
    "Mapping with multiple spans of the same variable returns the correct "
    "map") {
  auto mapping = Mapping();
  add_annotations(mapping, {{std::bitset<64>("1010"), 8},
                            {std::bitset<64>("0100"), 7},
                            {std::bitset<64>("1000"), 4},
                            {std::bitset<64>("0101"), 3}});

  std::map<int, std::vector<Span>> mappings_map = mapping.construct_mapping();
  std::map<int, std::vector<Span>> expected_map = {
      {0, {Span{3, 8}}}, {1, {Span{7, 8}, Span{3, 4}}}};

  REQUIRE(mappings_map == expected_map);
}

}  // namespace REmatch::testing
