#include <REmatch/query.hpp>
#include <REmatch/query_data.hpp>

#include "evaluation/document.hpp"
#include "evaluation/start_end_chars.hpp"
#include "mediator/mediator/findone_mediator.hpp"
#include "mediator/output_checker.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"

namespace REmatch {
inline namespace library_interface {

Query::Query(const std::string& pattern, Flags flags)
    : flags_(flags), query_data_(get_query_data(pattern, flags)) {}

std::unique_ptr<Match> Query::findone(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  std::unique_ptr<FindoneMediator> mediator =
      std::make_unique<FindoneMediator>(query_data_, document, flags_);

  mediator::Mapping* mapping = mediator->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, query_data_.variable_catalog,
                                   document);
  }

  return nullptr;
}

std::unique_ptr<MatchIterator> Query::finditer(const std::string& text) {
  return std::make_unique<MatchIterator>(query_data_, text, flags_);
}

bool Query::check(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto output_checker = OutputChecker(query_data_, document);
  return output_checker.check();
}

size_t Query::count(const std::string&) {
  return 0;
}
}  // namespace library_interface
}  // namespace REmatch