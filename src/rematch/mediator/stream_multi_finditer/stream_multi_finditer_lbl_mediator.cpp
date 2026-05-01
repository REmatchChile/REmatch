#include "stream_multi_finditer_lbl_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/line.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mediator.hpp"

using namespace REmatch;

StreamMultiFinditerLblMediator::StreamMultiFinditerLblMediator(
    QueryData& query_data, std::shared_ptr<Stream> stream,
    std::unique_ptr<LineSplitterStream> line_splitter,
    std::unique_ptr<SegmentCheckerStream> segment_checker)
    : line_splitter(std::move(line_splitter)),
      segment_checker(std::move(segment_checker)),
      stream(std::move(stream)) {

  algorithm = std::make_unique<FinditerAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);

  std::unique_ptr<Span> line = this->line_splitter->get_line();

  if (this->segment_checker->check(*line)) {
    update_algorithm(*line);
    algorithm->evaluate_start_char();
  }
}

std::unique_ptr<ExtendedMapping> StreamMultiFinditerLblMediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }
  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  extended_mapping->shift(static_cast<int64_t>(line_offset));
  return extended_mapping;
}

bool StreamMultiFinditerLblMediator::next_is_computed_successfully() {
  while (keep) {
    mapping_ = algorithm->next();
    if (mapping_ != nullptr) {
      return true;
    }

    keep = false;
    while (std::unique_ptr<Span> line = line_splitter->get_line()) {
      if (segment_checker->check(*line)) {
        update_algorithm(*line);
        algorithm->evaluate_start_char();
        keep = true;
        break;
      }
    }
  }

  return false;
}

void StreamMultiFinditerLblMediator::update_algorithm(Span& span) {
  line_offset = span.first;
  std::string_view line = stream->get_segment(span);
  algorithm->begin(std::make_shared<Line>(line));
}
