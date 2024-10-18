#include "output_enumeration/extended_mapping.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

namespace REmatch::testing {

TEST_CASE("Extended mapping without annotations results in an empty map") {
  auto mapping = ExtendedMapping(std::vector<Mapping::Annotation>());

  std::map<int, std::vector<Span>> mappings_map = mapping.construct_mapping();
  REQUIRE(mappings_map.size() == 0);
}

TEST_CASE(
    "Extended mapping with multiple spans of the same variable returns the "
    "correct map") {
  std::vector<Mapping::Annotation> annotations = {{std::bitset<64>("1010"), 8},
                                                  {std::bitset<64>("0100"), 7},
                                                  {std::bitset<64>("1000"), 4},
                                                  {std::bitset<64>("0101"), 3}};
  auto extended_mapping = ExtendedMapping(std::move(annotations));

  std::map<int, std::vector<Span>> expected_map = {
      {0, {Span{3, 8}}},
      {1, {Span{3, 4}, Span{7, 8}}},
  };

  std::map<int, std::vector<Span>> actual_map =
      extended_mapping.construct_mapping();

  REQUIRE(actual_map == expected_map);
}

TEST_CASE(
    "Extended mapping with a variable that is opened and closed in the same "
    "position") {
  std::vector<Mapping::Annotation> annotations = {{std::bitset<64>("1010"), 4},
                                                  {std::bitset<64>("1100"), 3},
                                                  {std::bitset<64>("1100"), 2},
                                                  {std::bitset<64>("0101"), 1}};
  auto extended_mapping = ExtendedMapping(std::move(annotations));

  std::map<int, std::vector<Span>> expected_map = {
      {0, {Span{1, 4}}},
      {1, {Span{1, 2}, Span{2, 3}, Span{3, 4}}},
  };

  std::map<int, std::vector<Span>> actual_map =
      extended_mapping.construct_mapping();

  REQUIRE(actual_map == expected_map);
}

TEST_CASE("the method get_submapping returns the correct extended mapping") {
  auto extended_mapping = ExtendedMapping({{std::bitset<64>("1010"), 4},
                                           {std::bitset<64>("1100"), 3},
                                           {std::bitset<64>("1100"), 2},
                                           {std::bitset<64>("0101"), 1}});

  auto submapping = extended_mapping.get_submapping({2, 3});

  auto expected_extended_mapping = ExtendedMapping(
      {{std::bitset<64>("1100"), 3}, {std::bitset<64>("1100"), 2}});

  REQUIRE(*submapping == expected_extended_mapping);
}

TEST_CASE(
    "the method get_submapping returns the correct extended mapping when "
    "positions are not in the annotations vector") {
  auto extended_mapping = ExtendedMapping({{std::bitset<64>("1010"), 8},
                                           {std::bitset<64>("1100"), 6},
                                           {std::bitset<64>("1100"), 4},
                                           {std::bitset<64>("0101"), 2}});
  auto submapping_exact_lower = extended_mapping.get_submapping({4, 7});
  auto submapping_exact_upper = extended_mapping.get_submapping({3, 6});

  auto expected_submapping = extended_mapping.get_submapping({4, 6});

  REQUIRE(*expected_submapping == *submapping_exact_lower);
  REQUIRE(*expected_submapping == *submapping_exact_upper);
}

}  // namespace REmatch::testing
