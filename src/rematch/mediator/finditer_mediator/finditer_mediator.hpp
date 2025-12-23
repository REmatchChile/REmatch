#pragma once

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "filtering_module/segment_identificator_base.hpp"
#include "mediator/mapping.hpp"
#include "mediator/mediator.hpp"
#include "utils/query_data.hpp"

#include <REmatch/constants.hpp>

namespace REmatch {

class FinditerMediator : public Mediator {
 public:
  FinditerMediator(QueryData& query_data, std::shared_ptr<Document> document,
                   std::unique_ptr<SegmentIdentificatorBase> segment_identificator);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& segment_span);

  std::unique_ptr<SegmentIdentificatorBase> segment_identificator;
  std::unique_ptr<FinditerAlgorithm> algorithm_;
};

}  // namespace REmatch
