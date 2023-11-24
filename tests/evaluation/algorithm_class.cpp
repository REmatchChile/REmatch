#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "mapping_helpers.hpp"

namespace rematch::testing {

ExtendedDetVA get_extended_det_va_from_regex(std::string_view input);
ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);
void run_algorithm_test(std::string regex, std::string document,
                        std::vector<DummyMapping> expected_mappings);
std::string get_mapping_info(DummyMapping mapping);
std::string create_document_with_repeated_string(std::string_view string, int times);

TEST_CASE("the algorithm returns a null pointer if there are no mappings") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("!x{a}");
  std::string document = "b";
  document += END_CHAR;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the algorithm returns an empty mapping if there are no captures") {
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex("a");
  std::string document = "a";
  document += END_CHAR;

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping != nullptr);

  mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the algorithm returns correct mappings for '!x{a}' over 'aaa'") {
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {0, 1}}}),
    DummyMapping({{"x", {1, 2}}}),
    DummyMapping({{"x", {2, 3}}}),
  };
  run_algorithm_test("!x{a}", "aaa", expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when using quantifiers") {
  std::vector<DummyMapping> expected_mappings;

  SECTION("quantifier +") {
    expected_mappings = {
      DummyMapping({{"x", {0, 1}}}),
      DummyMapping({{"x", {1, 2}}}),
      DummyMapping({{"x", {0, 2}}})
    };
    run_algorithm_test("!x{a+}", "aa", expected_mappings);
  }

  SECTION("quantifier ?") {
    expected_mappings = {
      DummyMapping({{"x", {1, 3}}}),
      DummyMapping({{"x", {0, 3}}}),
      DummyMapping({{"x", {5, 7}}}),
      DummyMapping({{"x", {4, 7}}}),
      DummyMapping({{"x", {5, 8}}})
    };
    run_algorithm_test("!x{a[ab]?b}", "aabaaabba", expected_mappings);
  }

  SECTION("quantifier *") {
    expected_mappings = {
      DummyMapping({{"x", {0, 1}}}), DummyMapping({{"x", {3, 4}}}),
      DummyMapping({{"x", {0, 4}}}), DummyMapping({{"x", {6, 7}}}),
      DummyMapping({{"x", {3, 7}}}), DummyMapping({{"x", {0, 7}}}),
      DummyMapping({{"x", {9, 10}}}), DummyMapping({{"x", {6, 10}}}),
      DummyMapping({{"x", {3, 10}}}), DummyMapping({{"x", {0, 10}}})
    };
    run_algorithm_test("!x{t(hat)*}", "thathathat", expected_mappings);
  }
}

TEST_CASE("the algorithm returns correct mappings when using nested variables") {
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {0, 1}}, {"y", {0, 1}}}),
    DummyMapping({{"x", {1, 2}}, {"y", {1, 2}}})
  };
  run_algorithm_test("!x{(!y{a}b|!y{a})}", "aa", expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings for '!x{!y{a}.+}'") {

  SECTION("over 'aaa'") {
    std::vector<DummyMapping> expected_mappings = {
      DummyMapping({{"x", {0, 2}}, {"y", {0, 1}}}),
      DummyMapping({{"x", {1, 3}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 3}}, {"y", {0, 1}}})
    };
    run_algorithm_test("!x{!y{a}.+}", "aaa", expected_mappings);
  }

  SECTION("over 'ab ab'") {
    std::vector<DummyMapping> expected_mappings = {
      DummyMapping({{"x", {0, 2}}, {"y", {0, 1}}}),
      DummyMapping({{"x", {0, 3}}, {"y", {0, 1}}}),
      DummyMapping({{"x", {0, 4}}, {"y", {0, 1}}}),
      DummyMapping({{"x", {3, 5}}, {"y", {3, 4}}}),
      DummyMapping({{"x", {0, 5}}, {"y", {0, 1}}})
    };
    run_algorithm_test("!x{!y{a}.+}", "ab ab", expected_mappings);
  }
}

TEST_CASE("the algorithm returns correct mappings when using char classes") {
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"z", {3, 8}}}),
    DummyMapping({{"z", {11, 16}}}),
    DummyMapping({{"z", {22, 27}}})
  };
  std::string document = "...Optimal optimistic optimizations...";
  run_algorithm_test("!z{[Oo]ptim}", document, expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings when using negation") {
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"w", {0, 1}}}), DummyMapping({{"w", {1, 2}}}), DummyMapping({{"w", {0, 2}}}),
    DummyMapping({{"w", {2, 3}}}), DummyMapping({{"w", {1, 3}}}), DummyMapping({{"w", {0, 3}}}),
    DummyMapping({{"w", {3, 4}}}), DummyMapping({{"w", {2, 4}}}), DummyMapping({{"w", {1, 4}}}),
    DummyMapping({{"w", {0, 4}}}), DummyMapping({{"w", {5, 6}}}), DummyMapping({{"w", {6, 7}}}),
    DummyMapping({{"w", {5, 7}}}), DummyMapping({{"w", {7, 8}}}), DummyMapping({{"w", {6, 8}}}),
    DummyMapping({{"w", {5, 8}}}), DummyMapping({{"w", {8, 9}}}), DummyMapping({{"w", {7, 9}}}),
    DummyMapping({{"w", {6, 9}}}), DummyMapping({{"w", {5, 9}}})
  };
  run_algorithm_test("!w{[^\\s]+}", "stay safe", expected_mappings);
}

TEST_CASE("the algorithm returns correct mappings") {
  std::string regex = "section\\*?\\{!x{.+}\\}";
  std::string document = "...\\subsection*{Introduction}...";
  std::vector<DummyMapping> expected_mappings = {
    DummyMapping({{"x", {16, 28}}})
  };
  run_algorithm_test(regex, document, expected_mappings);
}

TEST_CASE("nodes used by the algorithm are recycled when creating a linked list") {
  // for each character, the algorithm adds 3 nodes to the ecs
  int size = (MEMORY_POOL_STARTING_SIZE + 1) / 3;
  std::string document = create_document_with_repeated_string("a", size);

  std::string regex = "!x{a+}";
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex(regex);

  ECS ecs = ECS();
  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);
  algorithm.set_ecs(ecs);

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  CHECK(ecs.get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);

  // -1 because it creates a linked list with depth+1 nodes
  create_linked_list_node_of_depth(&ecs, MEMORY_POOL_STARTING_SIZE - 1);
  REQUIRE(ecs.amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

TEST_CASE("nodes used by the algorithm are recycled when it is run again") {
  std::string document = create_document_with_repeated_string("aaac", 300);

  std::string regex = "!x{a+}";
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex(regex);

  ECS ecs = ECS();
  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);
  algorithm.set_ecs(ecs);

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  CHECK(ecs.get_amount_of_nodes_used() >= MEMORY_POOL_STARTING_SIZE);

  // run the algorithm again and verify that the nodes are reused
  algorithm.initialize_algorithm();

  mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  REQUIRE(ecs.amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

TEST_CASE("nodes used by the algorithm are recycled when, after constructing the \
           ECS, there is no mapping") {
  // create nodes leaving 5 free nodes in the pool
  // the ecs should allocate no more than 5 nodes
  ECS ecs = ECS();
  for (int i = 0; i < MEMORY_POOL_STARTING_SIZE - 5; i++)
    ecs.create_bottom_node();

  // the document needs 6 nodes: bottom, open x at 0, at 1, at 2, union of open x
  // at 0 and open x at 1, and union of previous union and open x at 2
  // note that the bottom node is created by the original ecs, not the one passed in the setter
  std::string document = create_document_with_repeated_string("aaac", 3);

  std::string regex = "!x{a+b}";
  ExtendedDetVA extended_det_va = get_extended_det_va_from_regex(regex);

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);
  algorithm.set_ecs(ecs);

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  CHECK(ecs.get_amount_of_nodes_used() >= MEMORY_POOL_STARTING_SIZE);
  REQUIRE(ecs.amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

void run_algorithm_test(std::string regex, std::string document,
                        std::vector<DummyMapping> expected_mappings) {
  document += END_CHAR;
  Parser parser = Parser(regex);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  auto extended_det_va = ExtendedDetVA(extended_va);
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();

  AlgorithmClass algorithm = AlgorithmClass(extended_det_va, document);

  std::ostringstream info_os;
  info_os << "Actual mappings:\n";
  const Mapping* result_mapping = algorithm.get_next_mapping();

  while (result_mapping != nullptr) {
    int number_of_variables = expected_mappings[0].number_of_variables;

    DummyMapping actual_mapping({});

    for (int variable_id = 0; variable_id < number_of_variables; variable_id++) {
      std::vector<Span> spans = result_mapping->get_spans_of_variable_id(variable_id);

      std::string variable_name = variable_catalog->get_var(variable_id);
      actual_mapping.add_span(variable_name, spans.back());
    }
    info_os << get_mapping_info(actual_mapping);
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_mappings, actual_mapping));
    remove_mapping_from_expected(expected_mappings, actual_mapping);

    result_mapping = algorithm.get_next_mapping();
  }

  INFO(info_os.str());
  REQUIRE(expected_mappings.size() == 0);
}

std::string get_mapping_info(DummyMapping mapping) {
  std::ostringstream os;
  for (auto& pair : mapping.spans_map) {
    os << pair.first << " -> " << pair.second << "\n";
  }
  return os.str();
}

std::string create_document_with_repeated_string(std::string_view string, int times) {
  std::ostringstream os;
  for (int i = 0; i < times; i++)
      os << string;

  return os.str() + END_CHAR;
}

}  // namespace rematch::testing
