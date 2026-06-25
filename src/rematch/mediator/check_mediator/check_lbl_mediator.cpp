#include "check_lbl_mediator.hpp"
#include "mediator/line_splitter.hpp"

using namespace REmatch;

CheckLblMediator::CheckLblMediator(QueryData& query_data, std::shared_ptr<Document> document,
                                   std::unique_ptr<SegmentChecker> checker)
    : Mediator(query_data), document(std::move(document)), checker(std::move(checker)) {}

std::unique_ptr<mediator::Mapping> CheckLblMediator::next() {
  if (has_returned) {
    return nullptr;
  }
  has_returned = true;

  auto line_splitter = LineSplitterStr(document);
  std::unique_ptr<Span> line = line_splitter.get_line();

  while (line != nullptr) {
    if (checker->check(*line)) {
      return std::make_unique<mediator::Mapping>();
    }
    line = line_splitter.get_line();
  }
  return nullptr;
}
