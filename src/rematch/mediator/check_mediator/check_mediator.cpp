#include "check_mediator.hpp"

#include <utility>

using namespace REmatch;

CheckMediator::CheckMediator(QueryData& query_data, std::shared_ptr<Document> document,
                             std::unique_ptr<SegmentChecker> checker)
    : Mediator(query_data), document(std::move(document)), checker(std::move(checker)) {}

std::unique_ptr<mediator::Mapping> CheckMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  if (checker->check({0, document->size()})) {
    return std::make_unique<mediator::Mapping>();
  }
  return nullptr;
}
