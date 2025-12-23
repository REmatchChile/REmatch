#include <REmatch/multi_query.hpp>

#include <cstdint>

#include "evaluation/document.hpp"
#include "mediator/mediator_constructor.hpp"
#include "utils/query_data.hpp"

#include <REmatch/flags.hpp>
#include <REmatch/multi_match.hpp>
#include <REmatch/multi_match_generator.hpp>

namespace REmatch {

inline namespace library_interface {

MultiQuery::MultiQuery(const std::string& pattern, Flags flags,
                       uint_fast32_t max_mempool_duplications,
                       uint_fast32_t max_deterministic_states)
    : query_data_(std::make_shared<QueryData>(
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

MultiMatch MultiQuery::findone(const std::string& document_) {
  auto document = std::make_shared<Document>(document_);

  auto mediator = MediatorConstructor::create_multi_findone_mediator(*query_data_, document);

  auto mapping = mediator->next();

  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  return {std::move(mapping), query_data_->variable_catalog, document};
}

std::vector<MultiMatch> MultiQuery::findmany(const std::string& document, uint_fast32_t limit) {
  std::vector<MultiMatch> res;

  const auto multi_match_generator = finditer(document);
  for (auto it = multi_match_generator.begin(); it != multi_match_generator.end() && limit > 0;
       ++it, --limit) {
    res.emplace_back(std::move(*it));
  }

  return res;
}

std::vector<MultiMatch> MultiQuery::findall(const std::string& document) {
  std::vector<MultiMatch> res;

  const auto multi_match_generator = finditer(document);
  for (auto&& match : multi_match_generator) {
    res.emplace_back(std::move(match));
  }

  return res;
}

MultiMatchGenerator MultiQuery::finditer(const std::string& document) {
  return {query_data_, std::make_shared<Document>(document)};
}

bool MultiQuery::check(const std::string& document_) {
  std::shared_ptr<Document> document = std::make_shared<Document>(document_);

  auto search_dfa = std::make_unique<SearchDFA>(query_data_->logical_va);
  SegmentChecker segment_checker(std::move(search_dfa), document);

  return segment_checker.check({0, document->size()});
}

std::vector<std::string> MultiQuery::variables() const {
  return query_data_->variable_catalog->variables();
}

}  // namespace library_interface
}  // namespace REmatch
