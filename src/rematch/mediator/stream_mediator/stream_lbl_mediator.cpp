#include "stream_lbl_mediator.hpp"

#include "evaluation/algorithm/stream_algorithm.hpp"
#include "evaluation/line.hpp"
#include "filtering_module/segment_identificator_stream.hpp"

namespace REmatch {

StreamLblMediator::StreamLblMediator(QueryData& query_data, std::shared_ptr<Stream> stream,
                                     std::unique_ptr<LineSplitterStream> line_splitter,
                                     std::unique_ptr<SegmentCheckerStream> segment_checker)
    : Mediator(query_data),
      variable_catalog_(query_data.variable_catalog),
      segment_checker(std::move(segment_checker)),
      line_splitter(std::move(line_splitter)),
      stream(std::move(stream)) {

  algorithm_ = std::make_unique<FinditerAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);

  std::unique_ptr<Span> line = this->line_splitter->get_line();

  if (line != nullptr && this->segment_checker->check(*line)) {
    update_algorithm(*line);
    algorithm_->evaluate_start_char();
  }
}

std::unique_ptr<mediator::Mapping> StreamLblMediator::next() {
#ifdef TRACY_ENABLE
  ZoneScopedNC("StreamMediator::next", 0x3c3836);
#endif
  if (next_is_computed_successfully()) {
    auto mapping = construct_user_mapping();
    mapping->shift(line_offset);
    return mapping;
  }

  return nullptr;
}

bool StreamLblMediator::next_is_computed_successfully() {
  while (keep) {
    mapping_ = algorithm_->next();
    if (mapping_ != nullptr) {
      return true;
    }

    keep = false;
    while (std::unique_ptr<Span> line = line_splitter->get_line()) {
      if (segment_checker->check(*line)) {
        update_algorithm(*line);

        algorithm_->evaluate_start_char();
        keep = true;
        break;
      }
    }
  }

  return false;
}

void StreamLblMediator::update_algorithm(Span& span) {
  line_offset = span.first;
  std::string_view line = stream->get_segment(span);
  algorithm_->begin(std::make_shared<Line>(line));
}

}  // namespace REmatch
