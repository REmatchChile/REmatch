#include "REmatch/multi_query.hpp"

#include <cstdint>

#include "REmatch/flags.hpp"
#include "REmatch/multi_match.hpp"
#include "evaluation/document.hpp"
#include "evaluation/stream.hpp"
#include "match/generator/multi_match_generator.hpp"
#include "match/generator/s_multi_match_generator.hpp"
#include "match/match/multi_match_standard.hpp"
#include "match/match/s_multi_match.hpp"
#include "mediator/mediator_constructor.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

inline namespace library_interface {

MultiQuery::MultiQuery(const std::string& pattern, Flags flags,
                       uint_fast32_t max_mempool_duplications,
                       uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size)
    : query_data_(std::make_shared<QueryData>(get_multi_query_data(
          pattern, flags, max_mempool_duplications, max_deterministic_states))),
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

std::optional<MultiMatch> MultiQuery::findone(const std::string& document_) const {
  auto document = std::make_shared<Document>(document_);
  auto mediator = MediatorConstructor::create_multi_findone_mediator(*query_data_, document);

  auto mapping = mediator->next();
  if (mapping == nullptr) {
    return std::nullopt;
  }

  auto match = std::make_unique<internal::MultiMatchStandard>(
      std::move(mapping), query_data_->variable_catalog, document);
  return MultiMatch(std::move(match));
}

std::optional<MultiMatch> MultiQuery::findone(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  auto mediator = MediatorConstructor::create_stream_multi_mediator(*query_data_, stream);

  auto mapping = mediator->next();
  if (mapping == nullptr) {
    return std::nullopt;
  }

  auto match = std::make_unique<internal::SMultiMatch>(std::move(mapping),
                                                       query_data_->variable_catalog, stream);
  return MultiMatch(std::move(match));
}

std::vector<MultiMatch> MultiQuery::findmany(const std::string& document,
                                             uint_fast32_t limit) const {
  std::vector<MultiMatch> res;

  const auto multi_match_generator = finditer(document);
  for (auto it = multi_match_generator.begin(); it != multi_match_generator.end() && limit > 0;
       ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<MultiMatch> MultiQuery::findmany(Reader* reader, uint_fast32_t limit) const {
  std::vector<MultiMatch> res;

  const auto multi_match_generator = finditer(reader);
  for (auto it = multi_match_generator.begin(); it != multi_match_generator.end() && limit > 0;
       ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<MultiMatch> MultiQuery::findall(const std::string& document) const {
  std::vector<MultiMatch> res;

  const auto multi_match_generator = finditer(document);
  for (const auto& match : multi_match_generator) {
    res.push_back(match);
  }

  return res;
}

std::vector<MultiMatch> MultiQuery::findall(Reader* reader) const {
  std::vector<MultiMatch> res;

  const auto multi_match_generator = finditer(reader);
  for (const auto& match : multi_match_generator) {
    res.push_back(match);
  }

  return res;
}

MultiMatchGenerator MultiQuery::finditer(const std::string& document) const {
  internal::MultiMatchGenerator match_generator(query_data_, std::make_shared<Document>(document));
  return MultiMatchGenerator(std::move(match_generator));
}

MultiMatchGenerator MultiQuery::finditer(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  internal::SMultiMatchGenerator match_generator(query_data_, stream);
  return MultiMatchGenerator(std::move(match_generator));
}

bool MultiQuery::check(const std::string& document_) const {
  auto document = std::make_shared<Document>(document_);

  auto search_dfa = std::make_unique<SearchDFA>(query_data_->logical_va);
  SegmentChecker segment_checker(std::move(search_dfa), document);

  if ((query_data_->flags & Flags::LINE_BY_LINE) != Flags::NONE) {
    auto line_splitter = LineSplitterStr(document);
    std::unique_ptr<Span> line = line_splitter.get_line();

    while (line != nullptr) {
      if (segment_checker.check(*line)) {
        return true;
      }
      line = line_splitter.get_line();
    }
    return false;

  } else {
    return segment_checker.check({0, document->size()});
  }
}

bool MultiQuery::check(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);

  auto search_dfa = std::make_unique<SearchDFA>(query_data_->logical_va);
  SegmentCheckerStream segment_checker(std::move(search_dfa), stream);

  if ((query_data_->flags & Flags::LINE_BY_LINE) != Flags::NONE) {
    auto line_splitter = LineSplitterStream(stream);
    std::unique_ptr<Span> line = line_splitter.get_line();

    while (line != nullptr) {
      if (segment_checker.check(*line)) {
        return true;
      }
      line = line_splitter.get_line();
    }
    return false;

  } else {
    return segment_checker.check();
  }
}

std::vector<std::string> MultiQuery::variables() const {
  return query_data_->variable_catalog->variables();
}

}  // namespace library_interface
}  // namespace REmatch
