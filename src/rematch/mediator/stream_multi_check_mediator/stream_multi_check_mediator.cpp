#include "stream_multi_check_mediator.hpp"

using namespace REmatch;

StreamMultiCheckMediator::StreamMultiCheckMediator(std::unique_ptr<SegmentCheckerStream> checker)
    : checker(std::move(checker)) {}

std::unique_ptr<ExtendedMapping> StreamMultiCheckMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  if (checker->check()) {
    return std::make_unique<ExtendedMapping>(mapping);
  }
  return nullptr;
}
