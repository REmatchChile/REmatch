#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"

namespace rematch::testing {

ExtendedDetVA get_extended_det_va_from_regex(std::string_view input);
ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);
void run_algorithm_test(std::string regex, std::string document,
                        std::vector<std::vector<Span>> expected_mappings);
std::string create_document_with_repeated_character(char character, int size);
std::string get_spans_info(Span actual, Span expected);
char EOF_char = (char) -1;

TEST_CASE("the algorithm returns a null pointer if there are no mappings") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{a}");
  std::string document = "b";
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the algorithm returns an empty mapping if there are no captures") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("a");
  std::string document = "a";
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping != nullptr);

  mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the algorithm returns correct mappings for '!x{a}' over 'aaa'") {
  std::vector<std::vector<Span>> expected_mappings = {{{0, 1}}, {{1, 2}}, {{2, 3}}};

  run_algorithm_test("!x{a}", "aaa", expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when using quantifiers") {
  std::vector<std::vector<Span>> expected_mappings;

  SECTION("quantifier +") {
    expected_mappings = {{{0, 1}}, {{1, 2}}, {{0, 2}}};
    run_algorithm_test("!x{a+}", "aa", expected_mappings);
  }

  SECTION("quantifier ?") {
    expected_mappings = {{{1, 3}}, {{0, 3}}, {{5, 7}}, {{4, 7}}, {{5, 8}}};
    run_algorithm_test("!x{a[ab]?b}", "aabaaabba", expected_mappings);
  }

  SECTION("quantifier *") {
    expected_mappings = {{{0, 1}}, {{3, 4}},  {{0, 4}},  {{6, 7}},  {{3, 7}},
                         {{0, 7}}, {{9, 10}}, {{6, 10}}, {{3, 10}}, {{0, 10}}};
    run_algorithm_test("!x{t(hat)*}", "thathathat", expected_mappings);
  }
}

TEST_CASE("the algorithm returns correct mappings when using nested variables") {
  std::vector<std::vector<Span>> expected_mappings = {
    {{0, 1}, {0, 1}},
    {{1, 2}, {1, 2}},
  };
  run_algorithm_test("!x{(!y{a}b|!y{a})}", "aa", expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings for '!x{!y{a}.+}'") {

  SECTION("over 'aaa'") {
    const std::vector<std::vector<Span>> expected_mappings = {
        {{0, 2}, {0, 1}},
        {{1, 3}, {1, 2}},
        {{0, 3}, {0, 1}}
    };
    run_algorithm_test("!x{!y{a}.+}", "aaa", expected_mappings);
  }

  SECTION("over 'ab ab'") {
    std::vector<std::vector<Span>> expected_mappings = {
        {{0, 2}, {0, 1}},
        {{0, 3}, {0, 1}},
        {{0, 4}, {0, 1}},
        {{3, 5}, {3, 4}},
        {{0, 5}, {0, 1}}
    };
    run_algorithm_test("!x{!y{a}.+}", "ab ab", expected_mappings);
  }
}

TEST_CASE("the algorithm returns correct mappings when using char classes") {
  std::vector<std::vector<Span>> expected_mappings = {
      {{3, 8}}, {{11, 16}}, {{22, 27}}
  };
  std::string document = "...Optimal optimistic optimizations...";
  run_algorithm_test("!z{[Oo]ptim}", document, expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when using negation") {
  std::vector<std::vector<Span>> expected_mappings = {
      {{0, 1}}, {{1, 2}}, {{0, 2}}, {{2, 3}}, {{1, 3}}, {{0, 3}}, {{3, 4}},
      {{2, 4}}, {{1, 4}}, {{0, 4}}, {{5, 6}}, {{6, 7}}, {{5, 7}}, {{7, 8}},
      {{6, 8}}, {{5, 8}}, {{8, 9}}, {{7, 9}}, {{6, 9}}, {{5, 9}}
  };
  run_algorithm_test("!w{[^\\s]+}", "stay safe", expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings") {
  std::string regex = "section\\*?\\{!x{.+}\\}";
  std::string document = "...\\subsection*{Introduction}...";
  std::vector<std::vector<Span>> expected_mappings = {{{16, 28}}};

  run_algorithm_test("section\\*?\\{!x{.+}\\}", document, expected_mappings);
}

TEST_CASE("nodes used by the algorithm are recycled when creating a linked list") {
  int size = 683;
  std::string document = create_document_with_repeated_character('a', size);

  std::string regex = "!x{a+}";
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex(regex);

  ECS ecs = ECS();
  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);
  algorithm.set_ecs(ecs);

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  CHECK(ecs.get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);

  create_linked_list_node_of_depth(&ecs, MEMORY_POOL_STARTING_SIZE);
  REQUIRE(ecs.amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

void run_algorithm_test(std::string regex, std::string document,
                        std::vector<std::vector<Span>> expected_mappings) {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex(regex);
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  for (auto expected_mapping : expected_mappings) {
    const Mapping* mapping = algorithm.get_next_mapping();
    REQUIRE(mapping != nullptr);

    int number_of_variables = expected_mapping.size();

    for (int variable_id = 0; variable_id < number_of_variables; variable_id++) {
      std::vector<Span> spans = mapping->get_spans_of_variable_id(variable_id);

      INFO(get_spans_info(spans.back(), expected_mapping[variable_id]));
      REQUIRE(spans.back() == expected_mapping[variable_id]);
    }
  }
  REQUIRE(algorithm.get_next_mapping() == nullptr);
}

std::string get_spans_info(Span actual, Span expected) {
  std::stringstream info;
  info << actual << " = " << expected;
  return info.str();
}

std::string create_document_with_repeated_character(char character, int size) {
  std::string document = std::string(size, character);
  document += EOF_char;
  return document;
}

}  // namespace rematch::testing
