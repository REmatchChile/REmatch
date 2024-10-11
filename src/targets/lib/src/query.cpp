#include <REmatch/query.hpp>

#include "evaluation/document.hpp"
#include "mediator/mediator/findone_mediator.hpp"
#include "mediator/output_checker.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {
inline namespace library_interface {

Query::Query(const std::string& pattern, Flags flags,
             uint_fast32_t max_mempool_duplications,
             uint_fast32_t max_deterministic_states)
    : query_data_(get_query_data(pattern, flags, max_deterministic_states)),
      max_mempool_duplications_(max_mempool_duplications),
      max_deterministic_states_(max_deterministic_states) {}

Query::Query(Query&& other) noexcept
    : query_data_(std::move(other.query_data_)),
      max_mempool_duplications_(other.max_mempool_duplications_),
      max_deterministic_states_(other.max_deterministic_states_) {}

Query::~Query() = default;

std::unique_ptr<Match> Query::findone(const std::string& document_) {
  auto document = std::make_shared<Document>(document_);

  auto mediator = std::make_unique<FindoneMediator>(*query_data_, document,
                                                    max_mempool_duplications_,
                                                    max_deterministic_states_);

  auto mapping = mediator->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(std::move(mapping),
                                   query_data_->variable_catalog, document);
  }

  return nullptr;
}

std::vector<std::unique_ptr<Match>> Query::findall(
    const std::string& document) {
  std::vector<std::unique_ptr<Match>> res;

  auto match_iterator = finditer(document);

  while (auto match = match_iterator->next()) {
    res.push_back(std::move(match));
  }

  return res;
}

std::unique_ptr<MatchIterator> Query::finditer(const std::string& document) {
  return std::make_unique<MatchIterator>(*query_data_, document,
                                         max_mempool_duplications_,
                                         max_deterministic_states_);
}

bool Query::check(const std::string& document_) {
  std::shared_ptr<Document> document = std::make_shared<Document>(document_);

  auto output_checker = OutputChecker(*query_data_, document);
  return output_checker.check();
}

}  // namespace library_interface
}  // namespace REmatch