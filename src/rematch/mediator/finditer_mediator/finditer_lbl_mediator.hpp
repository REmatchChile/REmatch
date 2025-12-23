#pragma once

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mediator.hpp"

namespace REmatch {

class FinditerLblMediator : public Mediator {
 public:
  FinditerLblMediator(QueryData& query_data, std::shared_ptr<Document> document,
                      std::unique_ptr<LineSplitterStr> line_splitter,
                      std::unique_ptr<SegmentChecker> segment_checker);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& span);

  bool keep = true;
  int64_t line_offset = 0;

  std::unique_ptr<LineSplitterStr> line_splitter;
  std::unique_ptr<SegmentChecker> segment_checker;
  std::unique_ptr<FinditerAlgorithm> algorithm;
  std::shared_ptr<Document> document;
};

}  // namespace REmatch
