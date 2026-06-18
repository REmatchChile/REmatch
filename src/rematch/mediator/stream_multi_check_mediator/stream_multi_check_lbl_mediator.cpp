#include "stream_multi_check_lbl_mediator.hpp"
#include "mediator/line_splitter.hpp"

using namespace REmatch;

StreamMultiCheckLblMediator::StreamMultiCheckLblMediator(std::shared_ptr<Stream> stream,
                                                         std::unique_ptr<SegmentCheckerStream> checker)
    : stream(std::move(stream)), checker(std::move(checker)) {}

std::unique_ptr<ExtendedMapping> StreamMultiCheckLblMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  auto line_splitter = LineSplitterStream(stream);
  std::unique_ptr<Span> line = line_splitter.get_line();

  while (line != nullptr) {
    if (checker->check(*line)) {
      return std::make_unique<ExtendedMapping>(mapping);
    }
    line = line_splitter.get_line();
  }
  return nullptr;
}
