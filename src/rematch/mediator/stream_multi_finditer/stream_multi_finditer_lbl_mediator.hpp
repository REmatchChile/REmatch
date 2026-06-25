
#pragma once

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mediator.hpp"
#include "mediator/multi_mediator.hpp"

namespace REmatch {

class StreamMultiFinditerLblMediator : public MultiMediator {
 public:
  StreamMultiFinditerLblMediator(QueryData& query_data, std::shared_ptr<Stream> stream,
                                 std::unique_ptr<LineSplitterStream> line_splitter,
                                 std::unique_ptr<SegmentCheckerStream> segment_checker);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& span);

  bool keep = true;
  uint64_t line_offset = 0;

  const Mapping* mapping_ = nullptr;

  std::unique_ptr<LineSplitterStream> line_splitter;
  std::unique_ptr<SegmentCheckerStream> segment_checker;
  std::unique_ptr<FinditerAlgorithm> algorithm;
  std::shared_ptr<Stream> stream;
};

}  // namespace REmatch
