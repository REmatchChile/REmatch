#include <REmatch/query.hpp>

#include <REmatch/flags.hpp>
#include <REmatch/match.hpp>
#include <REmatch/match_generator.hpp>
#include <stdexcept>

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
    : query_data_(std::make_shared<QueryData>(
          get_query_data(pattern, flags, max_deterministic_states))),
      max_mempool_duplications_(max_mempool_duplications),
      max_deterministic_states_(max_deterministic_states) {}

Query::Query(Query&& other) noexcept
    : query_data_(std::move(other.query_data_)),
      max_mempool_duplications_(other.max_mempool_duplications_),
      max_deterministic_states_(other.max_deterministic_states_) {}

Query& Query::operator=(Query&& other) noexcept {
  query_data_ = std::move(other.query_data_);
  max_mempool_duplications_ = other.max_mempool_duplications_;
  max_deterministic_states_ = other.max_deterministic_states_;
  return *this;
}

Query::~Query() = default;

Match Query::findone(const std::string& document_) const {
  auto document = std::make_shared<Document>(document_);

  auto mediator = std::make_unique<FindoneMediator>(*query_data_, document,
                                                    max_mempool_duplications_,
                                                    max_deterministic_states_);

  auto mapping = mediator->next();

  if (mapping == nullptr) {
    // TODO: Add correct exception/message
    throw std::runtime_error("No match found");
  }

  return {std::move(mapping), query_data_->variable_catalog, document};
}

std::vector<Match> Query::findmany(const std::string& document,
                                   uint_fast32_t limit) const {
  std::vector<Match> res;
  res.reserve(limit);

  const auto match_generator = finditer(document);
  for (auto it = match_generator.begin();
       it != match_generator.end() && limit > 0; ++it, --limit) {
    res.emplace_back(std::move(*it));
  }

  return res;
}

std::vector<Match> Query::findall(const std::string& document) const {
  std::vector<Match> res;

  const auto match_generator = finditer(document);
  for (auto&& match : match_generator) {
    res.emplace_back(std::move(match));
  }

  return res;
}

MatchGenerator Query::finditer(const std::string& document) const {
  return {query_data_, document, max_mempool_duplications_,
          max_deterministic_states_};
}

bool Query::check(const std::string& document_) {
  auto document = std::make_shared<Document>(document_);

  auto output_checker = OutputChecker(*query_data_, document);
  return output_checker.check();
}

std::vector<std::string> Query::variables() const {
  return query_data_->variable_catalog->variables();
}

}  // namespace library_interface
}  // namespace REmatch