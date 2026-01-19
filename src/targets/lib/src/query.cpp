#include "REmatch/query.hpp"

#include "REmatch/flags.hpp"
#include <stdexcept>

#include "REmatch/match_generator.hpp"
#include "REmatch/match_type_erased.hpp"
#include "REmatch/s_match_generator.hpp"
#include "evaluation/document.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_checker.hpp"
#include "match/standard/match_standard.hpp"
#include "match/stream/s_match.hpp"
#include "mediator/mediator_constructor.hpp"
#include "utils/query_data.hpp"

namespace REmatch {
inline namespace library_interface {

Query::Query(const std::string& pattern, Flags flags, uint_fast32_t max_mempool_duplications,
             uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size)
    : query_data_(
          std::make_shared<QueryData>(get_query_data(pattern, flags, max_deterministic_states))),
      max_mempool_duplications_(max_mempool_duplications),
      max_deterministic_states_(max_deterministic_states),
      buffer_size(buffer_size) {}

Query::Query(Query&& other) noexcept
    : query_data_(std::move(other.query_data_)),
      max_mempool_duplications_(other.max_mempool_duplications_),
      max_deterministic_states_(other.max_deterministic_states_),
      buffer_size(other.buffer_size) {}

Query& Query::operator=(Query&& other) noexcept {
  query_data_ = std::move(other.query_data_);
  max_mempool_duplications_ = other.max_mempool_duplications_;
  max_deterministic_states_ = other.max_deterministic_states_;
  buffer_size = other.buffer_size;
  return *this;
}

MatchTypeErased Query::findone(const std::string& document_) const {
  auto document = std::make_shared<Document>(document_);

  auto mediator = MediatorConstructor::create_findone_mediator(*query_data_, document);

  auto mapping = mediator->next();

  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  auto match =
      std::make_unique<MatchStandard>(std::move(mapping), query_data_->variable_catalog, document);
  return MatchTypeErased(std::move(match));
}

MatchTypeErased Query::findone(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  auto mediator = MediatorConstructor::create_stream_findone_mediator(*query_data_, stream);

  std::unique_ptr<mediator::Mapping> mapping = mediator->next();

  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  auto match = std::make_unique<SMatch>(std::move(mapping), query_data_->variable_catalog, stream);
  return MatchTypeErased(std::move(match));
}

std::vector<MatchTypeErased> Query::findmany(const std::string& document,
                                             uint_fast32_t limit) const {
  std::vector<MatchTypeErased> res;
  res.reserve(limit);

  const auto match_generator = finditer(document);
  for (auto it = match_generator.begin(); it != match_generator.end() && limit > 0; ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<MatchTypeErased> Query::findmany(Reader* reader, uint_fast32_t limit) const {
  std::vector<MatchTypeErased> res;
  res.reserve(limit);

  const auto match_generator = finditer(reader);
  for (auto it = match_generator.begin(); it != match_generator.end() && limit > 0; ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<MatchTypeErased> Query::findall(const std::string& document) const {
  std::vector<MatchTypeErased> res;

  const auto match_generator = finditer(document);
  for (auto& match : match_generator) {
    res.emplace_back(std::move(match));
  }

  return res;
}

std::vector<MatchTypeErased> Query::findall(Reader* reader) const {
  std::vector<MatchTypeErased> res;

  const auto match_generator = finditer(reader);
  for (auto& match : match_generator) {
    res.emplace_back(std::move(match));
  }

  return res;
}

MatchGeneratorTypeErased Query::finditer(const std::string& document) const {
  MatchGenerator match_generator{query_data_, std::make_shared<Document>(document)};
  return MatchGeneratorTypeErased(std::move(match_generator));
}

MatchGeneratorTypeErased Query::finditer(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  SMatchGenerator match_generator{query_data_, stream};
  return MatchGeneratorTypeErased(std::move(match_generator));
}

bool Query::check(const std::string& document_) {
  auto document = std::make_shared<Document>(document_);

  auto search_dfa = std::make_unique<SearchDFA>(query_data_->logical_va);
  SegmentChecker segment_checker(std::move(search_dfa), document);

  return segment_checker.check({0, document->size()});
}

bool Query::check(Reader* reader_) {
  auto stream = std::make_shared<Stream>(reader_, buffer_size);

  auto search_dfa = std::make_unique<SearchDFA>(query_data_->logical_va);
  SegmentCheckerStream segment_checker(std::move(search_dfa), stream);

  return segment_checker.check();
}

std::vector<std::string> Query::variables() const {
  return query_data_->variable_catalog->variables();
}

}  // namespace library_interface
}  // namespace REmatch