#pragma once

#include "evaluation/algorithm/stream_algorithm.hpp"
#include "evaluation/stream.hpp"
#include "filtering_module/segment_identificator_base.hpp"
#include "mediator/multi_mediator.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

class StreamMultiFinditerMediator : public MultiMediator {
 public:
  StreamMultiFinditerMediator(QueryData& query_data, std::shared_ptr<Stream> stream,
                              std::unique_ptr<SegmentIdentificatorStream> segment_identificator);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& segment_span, SegmentResult segment_result);

  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::unique_ptr<SegmentIdentificatorStream> segment_identificator;
  std::unique_ptr<StreamAlgorithm> algorithm_;

  const output_enumeration::Mapping* mapping_ = nullptr;
};

}  // namespace REmatch
