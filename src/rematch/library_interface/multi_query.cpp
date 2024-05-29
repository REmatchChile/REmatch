#include "multi_query.hpp"
#include "evaluation/document.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/multi_match_iterator.hpp"
#include "library_interface/query_data/query_data.hpp"
#include "library_interface/query_data/query_data_utils.hpp"
#include "mediator/mediator/multi_findone_mediator.hpp"
#include "mediator/output_checker.hpp"

namespace REMatch {
using namespace rematch;

MultiQuery::MultiQuery(std::string_view pattern, Flags flags)
    : flags_(flags), query_data_(get_multi_query_data(pattern, flags)) {}

std::unique_ptr<MultiMatch> MultiQuery::findone(std::string_view text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto mediator = MultiFindoneMediator(query_data_, document, flags_);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();

  if (mapping != nullptr) {
    return std::make_unique<MultiMatch>(*mapping, query_data_.variable_catalog,
                                        document);
  }
  return nullptr;
}

std::unique_ptr<REMatch::MultiMatchIterator> MultiQuery::finditer(
    std::string_view text) {
  return std::make_unique<REMatch::MultiMatchIterator>(query_data_, text,
                                                       flags_);
}

bool MultiQuery::check(std::string_view text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);
  auto output_checker = OutputChecker(query_data_, document);
  return output_checker.check();
}

}  // namespace REMatch
