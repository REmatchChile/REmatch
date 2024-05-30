#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "mapping_helpers.hpp"
#include "evaluation/document.hpp"

namespace rematch::testing {

ExtendedVA get_extended_va_from_query(std::string query);
ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);
void run_algorithm_test(std::string query, std::string document,
                        std::vector<DummyMapping> expected_mappings);
std::string get_mapping_info(DummyMapping mapping);
std::string create_document_with_repeated_string(std::string_view string, int times);

TEST_CASE("the algorithm returns a null pointer if there are no mappings") {
  ExtendedVA extended_va = get_extended_va_from_query("!x{a}");
  auto document = std::make_shared<Document>("b");

  auto algorithm = FinditerAlgorithm(extended_va, document);

  const Mapping* mapping = algorithm.get_next_mapping();
  REQUIRE(mapping == nullptr);
}

TEST_CASE("the algorithm returns an empty mapping if there are no captures") {
  ExtendedVA extended_va = get_extended_va_from_query("a");
  auto document = std::make_shared<Document>("a");

  auto algorithm = FinditerAlgorithm(extended_va, document);

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
  auto document_ = create_document_with_repeated_string("a", size);
  auto document = std::make_shared<Document>(document_);

  std::string regex = "!x{a+}";
  ExtendedVA extended_va = get_extended_va_from_query(regex);

  auto algorithm = FinditerAlgorithm(extended_va, document);
  ECS& ecs = algorithm.get_ecs();

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr) {
    mapping = algorithm.get_next_mapping();
  }

  CHECK(algorithm.get_amount_of_nodes_used() == MEMORY_POOL_STARTING_SIZE);

  // -2 because it creates a linked list with depth+1 nodes and
  // the bottom node is always in use by the algorithm
  create_linked_list_node_of_depth(&ecs, MEMORY_POOL_STARTING_SIZE - 2);
  REQUIRE(algorithm.get_amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

TEST_CASE("nodes used by the algorithm are recycled when it is run again") {
  auto document_ = create_document_with_repeated_string("aaac", 300);
  auto document = std::make_shared<Document>(document_);

  std::string regex = "!x{a+}";
  ExtendedVA extended_va = get_extended_va_from_query(regex);

  auto algorithm = FinditerAlgorithm(extended_va, document);

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  CHECK(algorithm.get_amount_of_nodes_used() >= MEMORY_POOL_STARTING_SIZE);

  // run the algorithm again and verify that the nodes are reused
  algorithm.initialize_algorithm();

  mapping = algorithm.get_next_mapping();
  while (mapping != nullptr)
    mapping = algorithm.get_next_mapping();

  REQUIRE(algorithm.get_amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

TEST_CASE("nodes used by the algorithm are recycled when, after constructing the \
           ECS, there is no mapping") {
  // the document needs 6 nodes: bottom, open x at 0, at 1, at 2, union of open x
  // at 0 and open x at 1, and union of previous union and open x at 2
  auto document_ = create_document_with_repeated_string("aaac", 3);
  auto document = std::make_shared<Document>(document_);

  std::string regex = "!x{a+b}";
  ExtendedVA extended_va = get_extended_va_from_query(regex);

  auto algorithm = FinditerAlgorithm(extended_va, document);
  ECS& ecs = algorithm.get_ecs();

  // create nodes leaving 6 free nodes in the pool
  // the ecs should allocate no more than 6 nodes
  for (size_t i = 0; i < MEMORY_POOL_STARTING_SIZE - 6; i++) {
    ecs.create_bottom_node();
  }

  const Mapping* mapping = algorithm.get_next_mapping();
  while (mapping != nullptr) {
    mapping = algorithm.get_next_mapping();
  }

  CHECK(ecs.get_amount_of_nodes_used() >= MEMORY_POOL_STARTING_SIZE);
  REQUIRE(ecs.amount_of_nodes_allocated() == MEMORY_POOL_STARTING_SIZE);
}

void run_algorithm_test(std::string query, std::string document_,
                        std::vector<DummyMapping> expected_mappings) {
  auto document = std::make_shared<Document>(document_);
  Parser parser = Parser(query);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();

  auto algorithm = FinditerAlgorithm(extended_va, document);

  std::ostringstream info_os;
  info_os << "Actual mappings:\n";
  const Mapping* result_mapping = algorithm.get_next_mapping();

  while (result_mapping != nullptr) {
    int number_of_variables = expected_mappings[0].number_of_variables;

    DummyMapping actual_mapping({});

    for (int variable_id = 0; variable_id < number_of_variables; variable_id++) {
      std::vector<Span> spans = result_mapping->get_spans_of_variable_id(variable_id);
      for (auto& span : spans) {
        --(span.first);
        --(span.second);
      }

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

  return os.str();
}

}  // namespace rematch::testing
