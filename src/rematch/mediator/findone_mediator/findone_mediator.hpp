#pragma once

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "filtering_module/segment_identificator_base.hpp"
#include "mediator/mapping.hpp"
#include "mediator/mediator.hpp"
#include "utils/query_data.hpp"

#include <REmatch/constants.hpp>

namespace REmatch {

class FindoneMediator : public Mediator {
 public:
  FindoneMediator(QueryData& query_data, std::shared_ptr<Document> document,
                  std::unique_ptr<SegmentIdentificatorBase> segment_identificator);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  std::unique_ptr<SegmentIdentificatorBase> segment_identificator;
  std::unique_ptr<FindoneAlgorithm> algorithm_;
  std::shared_ptr<Document> document_;

  void update_algorithm(Span& segment_span);
};

}  // namespace REmatch
