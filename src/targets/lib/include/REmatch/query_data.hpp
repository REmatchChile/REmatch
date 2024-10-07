#pragma once

#include <memory>
#include <string>

#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "flags.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {

struct QueryData {
  QueryData(SegmentManagerCreator&& segment_manager_creator,
            ExtendedVA&& extended_va,
            std::shared_ptr<VariableCatalog> variable_catalog)
      : segment_manager_creator(std::move(segment_manager_creator)),
        extended_va(std::move(extended_va)),
        variable_catalog(variable_catalog) {}

  SegmentManagerCreator segment_manager_creator;
  ExtendedVA extended_va;
  std::shared_ptr<VariableCatalog> variable_catalog;
};

inline QueryData construct_query_data(Parser& parser, Flags flags) {
  LogicalVA& logical_va = parser.get_logical_va();
  logical_va.remove_useless_anchors();
  std::shared_ptr<parsing::VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();

  auto segment_manager_creator = SegmentManagerCreator(logical_va, flags);

  return {std::move(segment_manager_creator), std::move(extended_va),
          variable_catalog};
}

inline QueryData get_query_data(const std::string& pattern, Flags flags) {
  Parser parser = Parser(pattern);
  return construct_query_data(parser, flags);
}

inline QueryData get_multi_query_data(const std::string& pattern, Flags flags) {
  Parser parser = Parser(pattern, true);
  return construct_query_data(parser, flags);
}

}  // namespace REmatch