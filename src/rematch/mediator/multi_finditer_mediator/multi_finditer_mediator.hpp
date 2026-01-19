#pragma once

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "filtering_module/segment_identificator_base.hpp"
#include "mediator/multi_mediator.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

class MultiFinditerMediator : public MultiMediator {
 public:
  MultiFinditerMediator(QueryData& query_data, std::shared_ptr<Document> document,
                        std::unique_ptr<SegmentIdentificatorBase> segment_identificator);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& segment_span);

  std::shared_ptr<Document> document_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::unique_ptr<SegmentIdentificatorBase> segment_identificator;
  std::unique_ptr<FinditerAlgorithm> algorithm_;

  const output_enumeration::Mapping* mapping_ = nullptr;
};

}  // namespace REmatch
