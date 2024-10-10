#pragma once

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/document.hpp"
#include "mediator/segment_manager/segment_manager.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "output_enumeration/mapping.hpp"
#include "utils/query_data.hpp"

#include <REmatch/constants.hpp>

namespace REmatch {

class MultiFindoneMediator {
 public:
  MultiFindoneMediator(QueryData& query_data,
                       std::shared_ptr<Document> document,
                       uint_fast32_t max_mempool_duplications=REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                       uint_fast32_t max_deterministic_states=REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  std::unique_ptr<ExtendedMapping> next();

 private:
  void update_algorithm(Span& segment_span);

  std::shared_ptr<Document> document_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::unique_ptr<SegmentManager> segment_manager_;
  std::unique_ptr<AlgorithmClass> algorithm_;
  int number_of_variables_;

  const output_enumeration::Mapping* mapping_;

  bool has_output;
};

}  // namespace REmatch
