#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"
#include "mediator/mapping.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "utils/query_data.hpp"

namespace REmatch::testing {
using namespace REmatch::library_interface;

TEST_CASE("match iterator returns the correct matches for a simple query") {
  std::string regex = "!x{a}";
  std::string document = "all matches";
  std::vector<mediator::Mapping> expected_matches = {
      mediator::Mapping({{"x", {0, 1}}}), mediator::Mapping({{"x", {5, 6}}})};
  run_match_iterator_test(regex, document, expected_matches);
}

TEST_CASE(
    "match iterator returns the correct matches when there is more than one "
    "variable") {
  std::string regex = "!a{a+}!b{b+}";
  std::string document = "aabb";

  std::vector<mediator::Mapping> expected_matches = {
      mediator::Mapping({{"a", {0, 2}}, {"b", {2, 4}}}),
      mediator::Mapping({{"a", {0, 2}}, {"b", {2, 3}}}),
      mediator::Mapping({{"a", {1, 2}}, {"b", {2, 4}}}),
      mediator::Mapping({{"a", {1, 2}}, {"b", {2, 3}}})};
  run_match_iterator_test(regex, document, expected_matches);
}

}  // namespace REmatch::testing
