#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"
#include "evaluation/document.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "mediator/mediator/finditer_mediator.hpp"

namespace REmatch::testing {

TEST_CASE("the mediator returns null pointer when there are no mappings") {
  Parser parser = Parser("!x{a}");
  auto document = std::make_shared<Document>("b");

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(
      logical_va, Flags::NONE, REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  FinditerMediator mediator = FinditerMediator(regex_data, document);

  auto mapping = mediator.next();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the mediator returns an empty mapping if there are no captures") {
  Parser parser = Parser("a");
  auto document = std::make_shared<Document>("a");

  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(
      logical_va, Flags::NONE, REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  FinditerMediator mediator = FinditerMediator(regex_data, document);

  auto mapping = mediator.next();
  REQUIRE(mapping->get_spans_map().empty());
}

TEST_CASE("the mediator returns the correct mappings for !x{a} over aaa") {
  std::string document = "aaa";
  std::string regex = "!x{a}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {0, 1}}}), mediator::Mapping({{"x", {1, 2}}}),
      mediator::Mapping({{"x", {2, 3}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE("the mediator returns the correct mappings when using quantifier +") {
  std::string document = "aa aa";
  std::string regex = "!x{a+}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {0, 1}}}), mediator::Mapping({{"x", {1, 2}}}),
      mediator::Mapping({{"x", {0, 2}}}), mediator::Mapping({{"x", {3, 4}}}),
      mediator::Mapping({{"x", {4, 5}}}), mediator::Mapping({{"x", {3, 5}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE("the mediator returns the correct mappings when using quantifier ?") {
  std::string document = "aabaaabba";
  std::string regex = "!x{a[ab]?b}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {1, 3}}}), mediator::Mapping({{"x", {0, 3}}}),
      mediator::Mapping({{"x", {5, 7}}}), mediator::Mapping({{"x", {4, 7}}}),
      mediator::Mapping({{"x", {5, 8}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE("the mediator returns the correct mappings when using quantifier *") {
  std::string document = "thathathat";
  std::string regex = "!x{t(hat)*}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {0, 1}}}),  mediator::Mapping({{"x", {3, 4}}}),
      mediator::Mapping({{"x", {0, 4}}}),  mediator::Mapping({{"x", {6, 7}}}),
      mediator::Mapping({{"x", {3, 7}}}),  mediator::Mapping({{"x", {0, 7}}}),
      mediator::Mapping({{"x", {9, 10}}}), mediator::Mapping({{"x", {6, 10}}}),
      mediator::Mapping({{"x", {3, 10}}}), mediator::Mapping({{"x", {0, 10}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the mediator returns the correct mappings when using nested variables") {
  std::string document = "ab ab";
  std::string regex = "!x{!y{a}.+}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {0, 2}}, {"y", {0, 1}}}),
      mediator::Mapping({{"x", {0, 3}}, {"y", {0, 1}}}),
      mediator::Mapping({{"x", {0, 4}}, {"y", {0, 1}}}),
      mediator::Mapping({{"x", {3, 5}}, {"y", {3, 4}}}),
      mediator::Mapping({{"x", {0, 5}}, {"y", {0, 1}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE("the mediator returns the correct mappings when using alternation") {
  std::string document = "Meat, vegetables and spices (or sauces).";
  std::string regex = "!condiment{sauce|spice|syrup}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"condiment", {21, 26}}}),
      mediator::Mapping({{"condiment", {32, 37}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE("the mediator returns the correct mappings when using char classes") {
  std::string document = "man pages are your friend";
  std::string regex = "!man{[a-z]{4}}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"man", {4, 8}}}),
      mediator::Mapping({{"man", {5, 9}}}),
      mediator::Mapping({{"man", {14, 18}}}),
      mediator::Mapping({{"man", {14, 18}}}),
      mediator::Mapping({{"man", {19, 23}}}),
      mediator::Mapping({{"man", {20, 24}}}),
      mediator::Mapping({{"man", {21, 25}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the mediator returns the correct mappings when using short hand char "
    "classes") {
  std::string document = "b1 0a2 ab";
  std::string regex = "!x{\\d\\s\\w\\D}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {1, 5}}}), mediator::Mapping({{"x", {5, 9}}})};

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the mediator returns the correct mappings when using short hand char classes \
           and kleene closure ") {
  std::string document = " a12 100 1 ";
  std::string regex = "!x{\\d{2}}\\s!y{\\d+}\\s";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {2, 4}}, {"y", {5, 8}}}),
      mediator::Mapping({{"x", {6, 8}}, {"y", {9, 10}}}),
  };

  run_mediator_test(regex, document, expected_mappings);
}

TEST_CASE(
    "the mediator returns the correct mappings when using different short hand char \
           classes and kleene closure") {
  std::string document = " a12 100 a ";
  std::string regex = "!x{\\w*\\d{2}}\\s!y{\\w+\\W}";
  std::vector<mediator::Mapping> expected_mappings = {
      mediator::Mapping({{"x", {2, 4}}, {"y", {5, 9}}}),
      mediator::Mapping({{"x", {1, 4}}, {"y", {5, 9}}}),
      mediator::Mapping({{"x", {6, 8}}, {"y", {9, 11}}}),
      mediator::Mapping({{"x", {5, 8}}, {"y", {9, 11}}})};

  run_mediator_test(regex, document, expected_mappings);
}

}  // namespace REmatch::testing
