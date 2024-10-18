#pragma once

#include <bitset>
#include <string>
#include <vector>

#include "../evaluation/dummy_mapping.hpp"
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "evaluation/extended_va/nfa/extended_va_state.hpp"
#include "mediator/mapping.hpp"
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/ecs_node.hpp"
#include "parsing/charclass.hpp"

#include <REmatch/REmatch.hpp>

namespace REmatch::testing {

ECSNode* create_linked_list_node_of_depth(ECS* ecs, int depth);

std::string get_mapping_info(const DummyMapping& mapping);

void run_algorithm_test(const std::string& query, const std::string& document_,
                        std::vector<DummyMapping> expected_mappings);

void run_match_iterator_test(const std::string& query,
                             const std::string& document,
                             std::vector<mediator::Mapping> expected_matches);

void run_mediator_test(const std::string& query, const std::string& document,
                       std::vector<mediator::Mapping> expected_mappings);

ExtendedVA get_extended_va_from_query(const std::string& query);

std::string create_document_with_repeated_string(const std::string& str,
                                                 int times);

bool extended_va_has_only_read_captures_transitions(
    ExtendedVA const& extended_va);

bool state_has_self_loop(ExtendedVAState* state);

std::bitset<64> get_close_code(std::bitset<64> code);

int get_max_id(ExtendedVA const& extended_va);

int get_min_id(ExtendedVA const& extended_va);

bool charclass_contains_every_character(CharClass charclass);

void shift_span(std::unique_ptr<Span>& span);

void run_client_test(MatchGenerator& match_generator,
                     std::vector<DummyMapping> expected_matches);

void run_multi_mediator_test(const std::string& query,
                             const std::string& document,
                             std::vector<ExtendedMapping> expected_mappings);

std::bitset<64> get_markers(const std::string& bits);

}  // namespace REmatch::testing