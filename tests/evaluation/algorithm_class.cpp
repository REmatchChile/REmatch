#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"

namespace rematch::testing {

ExtendedDetVA get_extended_det_va_from_regex(std::string_view input);
ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);
std::string create_document_with_repeated_character(char character, int size);
int get_number_of_valid_mappings(AlgorithmClass& algorithm);
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

TEST_CASE("correct number of mappings for '!x{a}' over 'a'") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{a}");
  std::string document = "a";
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  const Mapping* mapping;
  mapping = algorithm.get_next_mapping();
  REQUIRE(mapping != nullptr);

  mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the algorithm returns the correct number of mappings for '!x{a}' over 'aaa' ") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{a}");
  std::string document = "aaa";
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);
  REQUIRE(get_number_of_valid_mappings(algorithm) == 3);
}

TEST_CASE("the algorithm returns the correct number of mappings when using quantifiers") {

  SECTION("quantifier +") {
    ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{a+}");
    std::string document = "aa";
    document += EOF_char;

    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 3);
  }

  SECTION("quantifier ?") {
    ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{a[ab]?b}");
    std::string document = "aabaaabba";
    document += EOF_char;

    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 5);
  }

  SECTION("quantifier *") {
    ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{t(hat)*}");
    std::string document = "thathathat";
    document += EOF_char;

    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 10);
  }
}

TEST_CASE("the algorithm returns the correct number of mappings when using nested variables") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{(!y{a}b|!y{a})}");
  std::string document = "aa";
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  REQUIRE(get_number_of_valid_mappings(algorithm) == 2);
}

TEST_CASE("the algorithm returns the correct number of mappings for '!x{!y{a}.+}'") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{!y{a}.+}");

  SECTION("over 'aaa'") {
    std::string document = "aaa";
    document += EOF_char;
    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 3);
  }

  SECTION("over 'ab ab'") {
    std::string document = "ab ab";
    document += EOF_char;
    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 5);
  }
}

TEST_CASE("the algorithm returns the correct number of mappings when using char classes") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!z{[Oo]ptim}");

  std::string document = "...Optimal optimistic optimizations...";
  document += EOF_char;
  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  REQUIRE(get_number_of_valid_mappings(algorithm) == 3);
}

TEST_CASE("the algorithm returns the correct number of mappings when using negation") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!w{[^\\s]+}");

  SECTION("over 'stay safe'") {
    std::string document = "stay safe";
    document += EOF_char;
    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 20);
  }

  SECTION("over 'at home'") {
    std::string document = "at home";
    document += EOF_char;

    AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

    REQUIRE(get_number_of_valid_mappings(algorithm) == 13);
  }
}

TEST_CASE("the algorithm returns the correct number of mappings") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("section\\*?\\{!x{.+}\\}");
  std::string document = "...\\subsection*{Introduction}...";
  document += EOF_char;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  REQUIRE(get_number_of_valid_mappings(algorithm) == 1);
}

TEST_CASE("nodes used by the algorithm are recycled when creating a linked list") {
  int size = 683;
  std::string document = create_document_with_repeated_character('a', size);

  std::string regex = "!x{a+}";
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex(regex);

  ECS ecs = ECS();
  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);
  algorithm.set_ecs(ecs);

  get_number_of_valid_mappings(algorithm);
  CHECK(ecs.get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);

  algorithm.clear_state_node_links();

  create_linked_list_node_of_depth(&ecs, MEMORY_POOL_STARTING_SIZE);
  REQUIRE(ecs.amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

int get_number_of_valid_mappings(AlgorithmClass& algorithm) {

  const Mapping* mapping = algorithm.get_next_mapping();
  int mapping_count = 0;

  while (mapping != nullptr) {
    mapping_count++;
    mapping = algorithm.get_next_mapping();
  }

  return mapping_count;
}

std::string create_document_with_repeated_character(char character, int size) {
  std::string document = std::string(size, character);
  document += EOF_char;
  return document;
}

}  // namespace rematch::testing
