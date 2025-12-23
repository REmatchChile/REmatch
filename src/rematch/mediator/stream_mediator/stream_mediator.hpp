#pragma once

#include "evaluation/algorithm/stream_algorithm.hpp"
#include "filtering_module/segment_identificator_stream.hpp"
#include "mediator/mapping.hpp"
#include "mediator/mediator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

class StreamMediator : public Mediator {
 public:
  StreamMediator(QueryData& query_data, const std::shared_ptr<Stream>& stream,
                 std::unique_ptr<SegmentIdentificatorStream> segment_identificator);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& segment_span, SegmentResult segment_result);

  std::shared_ptr<VariableCatalog> variable_catalog_ = nullptr;
  std::unique_ptr<StreamAlgorithm> algorithm_ = nullptr;

  std::unique_ptr<SegmentIdentificatorStream> segment_identificator_;
};

}  // namespace REmatch
