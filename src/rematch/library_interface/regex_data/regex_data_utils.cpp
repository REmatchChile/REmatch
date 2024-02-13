#include "regex_data_utils.hpp"

namespace rematch {

RegexData get_regex_data(std::string_view pattern, Flags flags) {
  rematch::Parser parser = rematch::Parser(pattern);
  rematch::LogicalVA logical_va = parser.get_logical_va();
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

}  // namespace rematch
