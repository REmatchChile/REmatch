#pragma once

#include <memory>
#include <string>

#include <REmatch/flags.hpp>
#include <utility>
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {

struct QueryData {
  // TODO: avoid a copy of the lva
  QueryData(ExtendedVA&& extended_va, std::shared_ptr<VariableCatalog> variable_catalog,
            LogicalVA logical_va, Flags flags, uint64_t max_amount_of_states)
      : extended_va(std::move(extended_va)),
        variable_catalog(std::move(variable_catalog)),
        logical_va(logical_va),
        flags(flags),
        max_amount_of_states(max_amount_of_states) {}

  ExtendedVA extended_va;
  std::shared_ptr<VariableCatalog> variable_catalog;

  LogicalVA logical_va;
  Flags flags;
  uint64_t max_mempool_duplications;
  uint64_t max_amount_of_states;
};

inline QueryData construct_query_data(Parser& parser, Flags flags,
                                      uint_fast32_t max_amount_of_states) {
  LogicalVA& logical_va = parser.get_logical_va();
  logical_va.remove_useless_anchors();
  std::shared_ptr<parsing::VariableCatalog> variable_catalog = parser.get_variable_catalog();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();

  return {std::move(extended_va), std::move(variable_catalog), logical_va, flags,
          max_amount_of_states};
}

inline QueryData get_query_data(const std::string& pattern, Flags flags,
                                uint_fast32_t max_amount_of_states) {
  Parser parser = Parser(pattern);
  return construct_query_data(parser, flags, max_amount_of_states);
}

inline QueryData get_multi_query_data(const std::string& pattern, Flags flags,
                                      uint_fast32_t max_amount_of_states) {
  Parser parser = Parser(pattern, true);
  return construct_query_data(parser, flags, max_amount_of_states);
}

}  // namespace REmatch
