#include "query_data_utils.hpp"

namespace rematch {

QueryData construct_query_data(rematch::Parser& parser, Flags flags) {
  rematch::LogicalVA& logical_va = parser.get_logical_va();
  logical_va.remove_useless_anchors();
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  rematch::ExtendedVA extended_va = rematch::ExtendedVA(logical_va);
  extended_va.clean_for_determinization();

  auto segment_manager_creator =
      rematch::SegmentManagerCreator(logical_va, flags);

  return {std::move(segment_manager_creator), std::move(extended_va),
          variable_catalog};
}

QueryData get_query_data(std::string_view pattern, Flags flags) {
  rematch::Parser parser = rematch::Parser(pattern);
  return construct_query_data(parser, flags);
}

QueryData get_multi_query_data(std::string_view pattern, Flags flags) {
  rematch::Parser parser = rematch::Parser(pattern, true);
  return construct_query_data(parser, flags);
}

}  // namespace rematch
