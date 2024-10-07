#include <REmatch/multi_query.hpp>

#include <REmatch/flags.hpp>
#include <REmatch/query_data.hpp>

#include "evaluation/document.hpp"
#include "mediator/mediator/multi_findone_mediator.hpp"
#include "mediator/output_checker.hpp"

namespace REmatch {

MultiQuery::MultiQuery(const std::string& pattern, Flags flags)
    : flags_(flags), query_data_(get_multi_query_data(pattern, flags)) {}

std::unique_ptr<MultiMatch> MultiQuery::findone(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto mediator = MultiFindoneMediator(query_data_, document, flags_);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();

  if (mapping != nullptr) {
    return std::make_unique<MultiMatch>(*mapping, query_data_.variable_catalog,
                                        document);
  }
  return nullptr;
}

std::unique_ptr<MultiMatchIterator> MultiQuery::finditer(
    const std::string& text) {
  return std::make_unique<MultiMatchIterator>(query_data_, text, flags_);
}

bool MultiQuery::check(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);
  auto output_checker = OutputChecker(query_data_, document);
  return output_checker.check();
}

}  // namespace REmatch
