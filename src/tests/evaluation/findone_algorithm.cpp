#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "mapping_helpers.hpp"
#include "evaluation/document.hpp"

namespace REmatch::testing {

TEST_CASE("the findone algorithm returns nullptr if there are no matches") {
  std::string regex = "!x{ab}";
  auto document = std::make_shared<Document>("bbb");
  Parser parser(regex);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va(logical_va);
  extended_va.clean_for_determinization();

  auto algorithm = FindoneAlgorithm(extended_va, document);
  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the findone algorithm returns the correct match") {
  std::string regex = "!x{b+}";
  auto document = std::make_shared<Document>("abba");
  Parser parser(regex);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va(logical_va);
  extended_va.clean_for_determinization();

  auto algorithm = FindoneAlgorithm(extended_va, document);
  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping != nullptr);

  std::map<int, std::vector<Span>> result_mapping =
      mapping->construct_mapping();

  REQUIRE(result_mapping.size() == 1);
  REQUIRE(result_mapping.count(0) == 1);
  REQUIRE(result_mapping[0] == std::vector<Span>{{2, 3}});
}

}  // namespace REmatch::testing
