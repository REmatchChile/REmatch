#ifndef QUERY_DATA_HPP
#define QUERY_DATA_HPP

#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"

namespace rematch {

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

}  // namespace rematch

#endif
