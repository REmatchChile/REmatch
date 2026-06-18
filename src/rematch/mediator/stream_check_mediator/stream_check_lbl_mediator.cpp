#include "stream_check_lbl_mediator.hpp"

#include <utility>
#include "mediator/line_splitter.hpp"

using namespace REmatch;

StreamCheckLblMediator::StreamCheckLblMediator(QueryData& query_data,
                                               std::shared_ptr<Stream> stream,
                                               std::unique_ptr<SegmentCheckerStream> checker)
    : Mediator(query_data), stream(std::move(stream)), checker(std::move(checker)) {}

std::unique_ptr<mediator::Mapping> StreamCheckLblMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  auto line_splitter = LineSplitterStream(stream);
  std::unique_ptr<Span> line = line_splitter.get_line();

  while (line != nullptr) {
    if (checker->check(*line)) {
      return std::make_unique<mediator::Mapping>();
    }
    line = line_splitter.get_line();
  }
  return nullptr;
}
