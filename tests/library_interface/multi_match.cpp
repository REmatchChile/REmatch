#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/multi_match.hpp"

namespace rematch::testing {

TEST_CASE("the spans method works correctly") {
  std::string text = "Hi there";
  auto document = std::make_shared<Document>(text);
  auto variable_catalog = std::make_shared<VariableCatalog>();
  variable_catalog->add("x");

  std::vector<Mapping::Annotation> annotations = {{std::bitset<64>("10"), 9},
                                                  {std::bitset<64>("01"), 3},
                                                  {std::bitset<64>("10"), 2},
                                                  {std::bitset<64>("01"), 0}};
  auto extended_mapping = ExtendedMapping(annotations);
  auto match = MultiMatch(extended_mapping, variable_catalog, document);

  std::vector<Span> expected_spans = {{0, 2}, {3, 9}};

  REQUIRE(match.spans(0) == expected_spans);
  REQUIRE(match.spans("x") == expected_spans);
}

TEST_CASE("the groups method works correctly") {
  std::string text = "Hi there";
  auto document = std::make_shared<Document>(text);
  auto variable_catalog = std::make_shared<VariableCatalog>();
  variable_catalog->add("x");

  std::vector<Mapping::Annotation> annotations = {{std::bitset<64>("10"), 9},
                                                  {std::bitset<64>("01"), 3},
                                                  {std::bitset<64>("10"), 2},
                                                  {std::bitset<64>("01"), 0}};
  auto extended_mapping = ExtendedMapping(annotations);
  auto match = MultiMatch(extended_mapping, variable_catalog, document);

  std::vector<std::string> expected_spans = {"Hi", "there"};

  REQUIRE(match.groups(0) == expected_spans);
  REQUIRE(match.groups("x") == expected_spans);
}

TEST_CASE("the submatch method returns the correct multi match") {
  std::string text = "Hi there";
  auto document = std::make_shared<Document>(text);
  auto variable_catalog = std::make_shared<VariableCatalog>();
  variable_catalog->add("x");
  variable_catalog->add("y");

  auto extended_mapping = ExtendedMapping({{std::bitset<64>("1010"), 9},
                                           {std::bitset<64>("0011"), 3},
                                           {std::bitset<64>("0001"), 2},
                                           {std::bitset<64>("0100"), 0}});
  auto match = MultiMatch(extended_mapping, variable_catalog, document);

  auto expected_extended_mapping =
      ExtendedMapping({{std::bitset<64>("1010"), 9},
                       {std::bitset<64>("0011"), 3},
                       {std::bitset<64>("0001"), 2}});
  auto expected_submatch =
      MultiMatch(expected_extended_mapping, variable_catalog, document);

  MultiMatch submatch = match.submatch({2, 9});

  REQUIRE(submatch == expected_submatch);
}

TEST_CASE(
    "the groups method of a multi match obtained with submatch returns the "
    "correct results") {
  std::string text = "aabbcc";
  auto document = std::make_shared<Document>(text);
  auto variable_catalog = std::make_shared<VariableCatalog>();
  variable_catalog->add("x");
  variable_catalog->add("y");

  auto extended_mapping = ExtendedMapping({{std::bitset<64>("1010"), 6},
                                           {std::bitset<64>("0011"), 4},
                                           {std::bitset<64>("0011"), 2},
                                           {std::bitset<64>("0101"), 0}});
  auto match = MultiMatch(extended_mapping, variable_catalog, document);

  MultiMatch submatch = match.submatch({2, 6});

  REQUIRE(submatch.groups("x") == std::vector<std::string>{"bb", "cc"});
  REQUIRE(submatch.groups("y") == std::vector<std::string>{});
}

}  // namespace rematch::testing
