#include "REmatch/multi_query.hpp"

#include <cstdint>

#include "evaluation/document.hpp"
#include "evaluation/stream.hpp"
#include "mediator/mediator_constructor.hpp"
#include "utils/query_data.hpp"

#include "REmatch/flags.hpp"
#include "REmatch/multi_match_generator.hpp"
#include "REmatch/multi_match_type_erased.hpp"
#include "match/standard/multi_match_standard.hpp"
#include "match/stream/s_multi_match.hpp"

namespace REmatch {

inline namespace library_interface {

MultiQuery::MultiQuery(const std::string& pattern, Flags flags,
                       uint_fast32_t max_mempool_duplications,
                       uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size)
    : query_data_(std::make_shared<QueryData>(
          get_multi_query_data(pattern, flags, max_deterministic_states))),
      max_mempool_duplications_(max_mempool_duplications),
      max_deterministic_states_(max_deterministic_states),
      buffer_size(buffer_size) {}

MultiQuery::MultiQuery(MultiQuery&& other) noexcept
    : query_data_(std::move(other.query_data_)),
      max_mempool_duplications_(other.max_mempool_duplications_),
      max_deterministic_states_(other.max_deterministic_states_),
      buffer_size(other.buffer_size) {}

MultiQuery& MultiQuery::operator=(MultiQuery&& other) noexcept {
  query_data_ = std::move(other.query_data_);
  max_mempool_duplications_ = other.max_mempool_duplications_;
  max_deterministic_states_ = other.max_deterministic_states_;
  buffer_size = other.buffer_size;
  return *this;
}

MultiQuery::~MultiQuery() = default;

MultiMatchTypeErased MultiQuery::findone(const std::string& document_) {
  auto document = std::make_shared<Document>(document_);
  auto mediator = MediatorConstructor::create_multi_findone_mediator(*query_data_, document);

  auto mapping = mediator->next();
  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  auto match = std::make_unique<MultiMatchStandard>(std::move(mapping),
                                                    query_data_->variable_catalog, document);
  return MultiMatchTypeErased(std::move(match));
}

MultiMatchTypeErased MultiQuery::findone(Reader* reader) {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  auto mediator = MediatorConstructor::create_stream_multi_mediator(*query_data_, stream);

  auto mapping = mediator->next();
  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  auto match =
      std::make_unique<SMultiMatch>(std::move(mapping), query_data_->variable_catalog, stream);
  return MultiMatchTypeErased(std::move(match));
}

std::vector<MultiMatchTypeErased> MultiQuery::findmany(const std::string& document,
                                                       uint_fast32_t limit) {
  std::vector<MultiMatchTypeErased> res;

  const auto multi_match_generator = finditer(document);
  for (auto it = multi_match_generator.begin(); it != multi_match_generator.end() && limit > 0;
       ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<MultiMatchTypeErased> MultiQuery::findmany(Reader* reader, uint_fast32_t limit) {
  std::vector<MultiMatchTypeErased> res;

  const auto multi_match_generator = finditer(reader);
  for (auto it = multi_match_generator.begin(); it != multi_match_generator.end() && limit > 0;
       ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<MultiMatchTypeErased> MultiQuery::findall(const std::string& document) {
  std::vector<MultiMatchTypeErased> res;

  const auto multi_match_generator = finditer(document);
  for (const auto& match : multi_match_generator) {
    res.push_back(match);
  }

  return res;
}

std::vector<MultiMatchTypeErased> MultiQuery::findall(Reader* reader) {
  std::vector<MultiMatchTypeErased> res;

  const auto multi_match_generator = finditer(reader);
  for (const auto& match : multi_match_generator) {
    res.push_back(match);
  }

  return res;
}

MultiMatchGeneratorTypeErased MultiQuery::finditer(const std::string& document) {
  MultiMatchGenerator match_generator(query_data_, std::make_shared<Document>(document));
  return MultiMatchGeneratorTypeErased(std::move(match_generator));
}

MultiMatchGeneratorTypeErased MultiQuery::finditer(Reader* reader) {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  SMultiMatchGenerator match_generator(query_data_, stream);
  return MultiMatchGeneratorTypeErased(std::move(match_generator));
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
