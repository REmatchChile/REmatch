#include "multi_check_mediator.hpp"

using namespace REmatch;

MultiCheckMediator::MultiCheckMediator(std::shared_ptr<Document> document,
                                       std::unique_ptr<SegmentChecker> checker)
    : document(std::move(document)), checker(std::move(checker)) {}

std::unique_ptr<ExtendedMapping> MultiCheckMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  if (checker->check({0, document->size()})) {
    return std::make_unique<ExtendedMapping>(mapping);
  }
  return nullptr;
}
