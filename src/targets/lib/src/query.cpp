#include <sys/types.h>
#include <REmatch/query.hpp>
#include <REmatch/query_data.hpp>

#include "evaluation/document.hpp"
#include "mediator/mediator/findone_mediator.hpp"
#include "mediator/output_checker.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"

namespace REmatch {
inline namespace library_interface {

Query::Query(const std::string& pattern, Flags flags,
             uint_fast32_t max_mempool_duplications,
             uint_fast32_t max_deterministic_states)
    : query_data_(get_query_data(pattern, flags, max_deterministic_states)),
      max_mempool_duplications_(max_mempool_duplications),
      max_deterministic_states_(max_deterministic_states) {}

std::unique_ptr<Match> Query::findone(const std::string& text) {
  auto document = std::make_shared<Document>(text);

  auto mediator = std::make_unique<FindoneMediator>(query_data_, document,
                                                    max_mempool_duplications_,
                                                    max_deterministic_states_);

  mediator::Mapping* mapping = mediator->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(*mapping, query_data_.variable_catalog,
                                   document);
  }

  return nullptr;
}

std::vector<Match> Query::findall(const std::string& text) {
  std::vector<Match> res;

  auto match_iterator = finditer(text);

  while (auto match = match_iterator->next()) {
    res.push_back(*match);
  }

  return res;
}

std::unique_ptr<MatchIterator> Query::finditer(const std::string& text) {
  return std::make_unique<MatchIterator>(
      query_data_, text, max_mempool_duplications_, max_deterministic_states_);
}

bool Query::check(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto output_checker = OutputChecker(query_data_, document);
  return output_checker.check();
}

}  // namespace library_interface
}  // namespace REmatch