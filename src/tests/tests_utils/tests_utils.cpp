#include "tests_utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <memory>
#include <sstream>

#include "../evaluation/mapping_helpers.hpp"
#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/document.hpp"
#include "mediator/mediator/finditer_mediator.hpp"
#include "mediator/mediator/multi_finditer_mediator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "output_enumeration/mapping.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "parsing/parser.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/query_data.hpp"

namespace REmatch::testing {

ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth) {
  ECSNode* current = ecs->create_bottom_node();

  for (int d = 0; d < depth; ++d) {
    current = ecs->create_extend_node(current, d, d);
  }
  return current;
}

std::string get_mapping_info(const DummyMapping& mapping) {
  std::ostringstream os;
  for (auto& pair : mapping.spans_map) {
    os << pair.first << " -> |" << pair.second.first << ", "
       << pair.second.second << ">\n";
  }
  return os.str();
}

void run_algorithm_test(const std::string& query, const std::string& document_,
                        std::vector<DummyMapping> expected_mappings) {
  auto document = std::make_shared<Document>(document_);
  Parser parser = Parser(query);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();

  auto algorithm = FinditerAlgorithm(extended_va, document);

  std::ostringstream info_os;
  info_os << "Actual mappings:\n";
  const auto* result_mapping = algorithm.get_next_mapping();

  while (result_mapping != nullptr) {
    int number_of_variables = expected_mappings[0].number_of_variables;

    DummyMapping actual_mapping({});

    for (int variable_id = 0; variable_id < number_of_variables;
         variable_id++) {
      std::vector<Span> spans =
          result_mapping->get_spans_of_variable_id(variable_id);
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
  REQUIRE(expected_mappings.empty());
}

void run_match_iterator_test(const std::string& query_,
                             const std::string& document,
                             std::vector<mediator::Mapping> expected_matches) {

  auto query = reql(query_);
  auto match_generator = query.finditer(document);

  std::ostringstream info_os;
  info_os << "Actual mappings:" << std::endl;
  for (const auto& match : match_generator) {
    auto actual_match = mediator::Mapping(match.groupdict());
    info_os << actual_match;
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_matches, actual_match));
    remove_mapping_from_expected(expected_matches, actual_match);
  }
  INFO(info_os.str());
  REQUIRE(expected_matches.empty());
}

void run_mediator_test(const std::string& query, const std::string& document_,
                       std::vector<mediator::Mapping> expected_mappings) {
  Parser parser = Parser(query);
  auto document = std::make_shared<Document>(document_);

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

  std::ostringstream info_os;
  info_os << "Actual mappings:" << std::endl;

  auto actual_mapping = mediator.next();
  while (actual_mapping != nullptr) {
    info_os << *actual_mapping;
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_mappings, *actual_mapping));
    remove_mapping_from_expected(expected_mappings, *actual_mapping);
    actual_mapping = mediator.next();
  }

  INFO(info_os.str());
  REQUIRE(expected_mappings.empty());
}

ExtendedVA get_extended_va_from_query(const std::string& query) {
  Parser parser(query);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va(logical_va);
  extended_va.clean_for_determinization();
  return extended_va;
}

std::string create_document_with_repeated_string(const std::string& str,
                                                 int times) {
  std::ostringstream os;
  for (int i = 0; i < times; ++i)
    os << str;

  return os.str();
}

bool extended_va_has_only_read_captures_transitions(
    ExtendedVA const& extended_va) {
  for (auto& state : extended_va.states) {
    if (!(state->filters.empty() && state->backward_filters.empty() &&
          state->captures.empty() && state->backward_captures.empty()))
      return false;
  }

  return true;
}

std::bitset<64> get_close_code(std::bitset<64> code) {
  return code << 1;
}

bool state_has_self_loop(ExtendedVAState* state) {
  for (auto& read_capture : state->read_captures) {
    if (read_capture->next == state)
      return true;
  }

  return false;
}

int get_max_id(ExtendedVA const& extended_va) {
  unsigned int max_id = 0;

  for (auto& state : extended_va.states) {
    max_id = std::max(max_id, state->id);
  }

  return max_id;
}

int get_min_id(ExtendedVA const& extended_va) {
  unsigned int min_id = extended_va.states.front()->id;

  for (auto& state : extended_va.states) {
    min_id = std::min(min_id, state->id);
  }

  return min_id;
}

bool charclass_contains_every_character(CharClass charclass) {
  for (int i = 0; i < 0xFF; i++) {
    if (!charclass.contains((char)i))
      return false;
  }

  return true;
}

void shift_span(std::unique_ptr<Span>& span) {
  --(span->first);
  --(span->second);
}

void run_client_test(MatchGenerator& match_generator,
                     std::vector<DummyMapping> expected_matches) {
  std::ostringstream info_os;
  info_os << "Actual mappings:\n";

  for (const auto& match : match_generator) {
    DummyMapping mapping({});

    for (const auto& variable : match.variables()) {
      mapping.add_span(variable, match.span(variable));
    }
    info_os << get_mapping_info(mapping);
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_matches, mapping));
    remove_mapping_from_expected(expected_matches, mapping);
  }

  INFO(info_os.str());
  REQUIRE(expected_matches.empty());
}

void run_multi_mediator_test(const std::string& query,
                             const std::string& document_,
                             std::vector<ExtendedMapping> expected_mappings) {
  Parser parser = Parser(query, true);
  auto document = std::make_shared<Document>(document_);

  LogicalVA logical_va = parser.get_logical_va();

  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto segment_manager_creator = SegmentManagerCreator(
      logical_va, Flags::NONE, REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  QueryData regex_data{std::move(segment_manager_creator),
                       std::move(extended_va), variable_catalog};
  auto mediator = MultiFinditerMediator(regex_data, document);

  std::ostringstream info_os;
  info_os << "Actual mappings:" << std::endl;

  std::unique_ptr<ExtendedMapping> actual_mapping = mediator.next();
  while (actual_mapping != nullptr) {
    info_os << *actual_mapping << std::endl;
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_mappings, *actual_mapping));
    remove_mapping_from_expected(expected_mappings, *actual_mapping);
    actual_mapping = mediator.next();
  }

  INFO(info_os.str());
  REQUIRE(expected_mappings.size() == 0);
}

std::bitset<64> get_markers(const std::string& bits) {
  std::bitset<64> variable_markers(bits);
  int size = variable_markers.size();
  variable_markers[size - 1] = 1;
  variable_markers[size - 2] = 1;
  return variable_markers;
}
}  // namespace REmatch::testing