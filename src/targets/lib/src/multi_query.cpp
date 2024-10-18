#include <REmatch/multi_query.hpp>

#include <REmatch/flags.hpp>

#include <cstdint>

#include "evaluation/document.hpp"
#include "mediator/mediator/multi_findone_mediator.hpp"
#include "mediator/output_checker.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

MultiQuery::MultiQuery(const std::string& pattern, Flags flags,
                       uint_fast32_t max_mempool_duplications,
                       uint_fast32_t max_deterministic_states)
    : query_data_(std::make_unique<QueryData>(
          get_multi_query_data(pattern, flags, max_deterministic_states))),
      max_mempool_duplications_(max_mempool_duplications),
      max_deterministic_states_(max_deterministic_states) {}

MultiQuery::MultiQuery(MultiQuery&& other) noexcept
    : query_data_(std::move(other.query_data_)),
      max_mempool_duplications_(other.max_mempool_duplications_),
      max_deterministic_states_(other.max_deterministic_states_) {}

MultiQuery& MultiQuery::operator=(MultiQuery&& other) noexcept {
  query_data_ = std::move(other.query_data_);
  max_mempool_duplications_ = other.max_mempool_duplications_;
  max_deterministic_states_ = other.max_deterministic_states_;
  return *this;
}

MultiQuery::~MultiQuery() = default;

std::unique_ptr<MultiMatch> MultiQuery::findone(const std::string& document_) {
  std::shared_ptr<Document> document = std::make_shared<Document>(document_);

  auto mediator =
      MultiFindoneMediator(*query_data_, document, max_mempool_duplications_,
                           max_deterministic_states_);

  auto mapping = mediator.next();

  if (mapping != nullptr) {
    return std::make_unique<MultiMatch>(
        std::move(mapping), query_data_->variable_catalog, document);
  }
  return nullptr;
}

std::vector<std::unique_ptr<MultiMatch>> MultiQuery::findmany(
    const std::string& document, uint_fast32_t limit) {
  std::vector<std::unique_ptr<MultiMatch>> res;

  auto multi_match_iterator = finditer(document);

  while (limit > 0) {
    auto match = multi_match_iterator.next();
    if (match == nullptr) {
      break;
    }

    res.push_back(std::move(match));
    --limit;
  }

  return res;
}

std::vector<std::unique_ptr<MultiMatch>> MultiQuery::findall(
    const std::string& document) {
  std::vector<std::unique_ptr<MultiMatch>> res;

  auto multi_match_iterator = finditer(document);

  while (auto match = multi_match_iterator.next()) {
    res.push_back(std::move(match));
  }

  return res;
}

MultiMatchIterator MultiQuery::finditer(const std::string& document) {
  return {*query_data_, document, max_mempool_duplications_,
          max_deterministic_states_};
}

bool MultiQuery::check(const std::string& document_) {
  std::shared_ptr<Document> document = std::make_shared<Document>(document_);
  auto output_checker = OutputChecker(*query_data_, document);
  return output_checker.check();
}

}  // namespace REmatch
