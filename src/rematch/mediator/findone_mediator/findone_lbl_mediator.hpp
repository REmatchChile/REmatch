#pragma once

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mediator.hpp"

namespace REmatch {

class FindoneLblMediator : public Mediator {
 public:
  FindoneLblMediator(QueryData& query_data, std::shared_ptr<Document> document,
                     std::unique_ptr<LineSplitterStr> line_splitter,
                     std::unique_ptr<SegmentChecker> segment_checker);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& segment_span);

  std::unique_ptr<LineSplitterStr> line_splitter;
  std::unique_ptr<SegmentChecker> segment_checker;
  std::unique_ptr<FindoneAlgorithm> algorithm_;
  std::shared_ptr<Document> document_;
};

}  // namespace REmatch
