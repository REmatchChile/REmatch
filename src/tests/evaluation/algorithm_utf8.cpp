#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "dummy_mapping.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

TEST_CASE("the algorithm returns correct mappings when there are UTF-8 characters that use 2 bytes") {
  std::string regex = "!x{b}";
  std::string document = "äΔbÓ";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {4, 5}}})
  };
  CHECK(document.size() == 7);

  run_algorithm_test(regex, document, expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when there are UTF-8 characters that use 3 bytes") {
  std::string regex = "!x{Ω}";
  std::string document = "€あΩ✈";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {6, 9}}})
  };
  CHECK(document.size() == 12);

  run_algorithm_test(regex, document, expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when there are UTF-8 characters that use 4 bytes") {
  std::string regex = "!x{a}";
  std::string document = "𑄃a𐀁𐤇🌍";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {4, 5}}})
  };
  CHECK(document.size() == 17);

  run_algorithm_test(regex, document, expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when there are UTF-8 characters and + in regex") {
  std::string regex = "!x{🠆+}🌍";
  std::string document = "👋🠆🠆🌍";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {4, 12}}}),
    DummyMapping({{"x", {8, 12}}})
  };

  run_algorithm_test(regex, document, expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when there are UTF-8 characters and char classes") {
  std::string regex = "(i=!x{[0-9]} to !y{[^()]})";
  std::string document = "Σ(i=1 to ∞) i";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {5, 6}}, {"y", {10, 13}}})
  };

  run_algorithm_test(regex, document, expected_mappings);
}


TEST_CASE("the algorithm returns correct mappings when there are UTF-8 characters and alternation") {
  std::string regex = "!pet{🐵|🐶|🐸|🐱}";
  std::string document = "I have a 🐱, a 🐸, and a 🐶.";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"pet", {9, 13}}}),
    DummyMapping({{"pet", {17, 21}}}),
    DummyMapping({{"pet", {29, 33}}})
  };

  run_algorithm_test(regex, document, expected_mappings);
}

}
