#include "REmatch/query.hpp"

#include <stdexcept>
#include "REmatch/flags.hpp"

#include "REmatch/match.hpp"
#include "evaluation/document.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_checker.hpp"
#include "match/generator/match_standard_generator.hpp"
#include "match/generator/s_match_generator.hpp"
#include "match/match/match_standard.hpp"
#include "match/match/s_match.hpp"
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

Match Query::findone(const std::string& document_) const {
  auto document = std::make_shared<Document>(document_);

  auto mediator = MediatorConstructor::create_findone_mediator(*query_data_, document);

  auto mapping = mediator->next();

  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  auto match = std::make_unique<internal::MatchStandard>(std::move(mapping),
                                                         query_data_->variable_catalog, document);
  return Match(std::move(match));
}

Match Query::findone(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  auto mediator = MediatorConstructor::create_stream_findone_mediator(*query_data_, stream);

  std::unique_ptr<mediator::Mapping> mapping = mediator->next();

  if (mapping == nullptr) {
    throw REmatchException("No match found");
  }

  auto match =
      std::make_unique<internal::SMatch>(std::move(mapping), query_data_->variable_catalog, stream);
  return Match(std::move(match));
}

std::vector<Match> Query::findmany(const std::string& document, uint_fast32_t limit) const {
  std::vector<Match> res;
  res.reserve(limit);

  const auto match_generator = finditer(document);
  for (auto it = match_generator.begin(); it != match_generator.end() && limit > 0; ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<Match> Query::findmany(Reader* reader, uint_fast32_t limit) const {
  std::vector<Match> res;
  res.reserve(limit);

  const auto match_generator = finditer(reader);
  for (auto it = match_generator.begin(); it != match_generator.end() && limit > 0; ++it, --limit) {
    res.emplace_back(*it);
  }

  return res;
}

std::vector<Match> Query::findall(const std::string& document) const {
  std::vector<Match> res;

  const auto match_generator = finditer(document);
  for (auto& match : match_generator) {
    res.emplace_back(std::move(match));
  }

  return res;
}

std::vector<Match> Query::findall(Reader* reader) const {
  std::vector<Match> res;

  const auto match_generator = finditer(reader);
  for (auto& match : match_generator) {
    res.emplace_back(std::move(match));
  }

  return res;
}

MatchGenerator Query::finditer(const std::string& document) const {
  internal::MatchStandardGenerator match_generator{query_data_,
                                                   std::make_shared<Document>(document)};
  return MatchGenerator(std::move(match_generator));
}

MatchGenerator Query::finditer(Reader* reader) const {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  internal::SMatchGenerator match_generator{query_data_, stream};
  return MatchGenerator(std::move(match_generator));
}

bool Query::check(const std::string& document_) const {
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

bool Query::check(Reader* reader_) const {
  auto stream = std::make_shared<Stream>(reader_, buffer_size);

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

std::vector<std::string> Query::variables() const {
  return query_data_->variable_catalog->variables();
}

}  // namespace library_interface
}  // namespace REmatch