#pragma once

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mapping.hpp"
#include "mediator/mediator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

class StreamLblMediator : public Mediator {
 public:
  StreamLblMediator(QueryData& query_data, std::shared_ptr<Stream> stream,
                    std::unique_ptr<LineSplitterStream> line_splitter,
                    std::unique_ptr<SegmentCheckerStream> segment_checker);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& span);

  std::shared_ptr<VariableCatalog> variable_catalog_ = nullptr;
  std::unique_ptr<FinditerAlgorithm> algorithm_ = nullptr;

  int64_t line_offset = 0;

  std::unique_ptr<SegmentCheckerStream> segment_checker;
  std::unique_ptr<LineSplitterStream> line_splitter;
  std::shared_ptr<Stream> stream;

  bool keep = true;
};

}  // namespace REmatch
