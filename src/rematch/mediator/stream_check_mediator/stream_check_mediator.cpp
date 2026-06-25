#include "stream_check_mediator.hpp"

using namespace REmatch;

StreamCheckMediator::StreamCheckMediator(QueryData& query_data, std::unique_ptr<SegmentCheckerStream> checker)
    : Mediator(query_data), checker(std::move(checker)) {}

std::unique_ptr<mediator::Mapping> StreamCheckMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  if (checker->check()) {
    return std::make_unique<mediator::Mapping>();
  }
  return nullptr;
}
