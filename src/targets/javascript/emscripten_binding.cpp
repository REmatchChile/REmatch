#include <cstdint>
#include <fstream>

#include <emscripten/bind.h>

#include "REmatch/REmatch.hpp"

using namespace emscripten;
using namespace REmatch;

template <class IteratorType, class ValueType>
class EmscriptenIteratorWrapper {
 public:
  explicit EmscriptenIteratorWrapper(IteratorType&& it_) : it(std::move(it_)) {}

  ValueType get() { return *it; }

  bool hasValue() { return it.operator->() != nullptr; }

  void next() { ++it; }

 private:
  IteratorType it;
};

template class EmscriptenIteratorWrapper<Iterator, Match>;
template class EmscriptenIteratorWrapper<MultiIterator, MultiMatch>;

using MatchGeneratorWrapper = EmscriptenIteratorWrapper<Iterator, Match>;
using MultiMatchGeneratorWrapper = EmscriptenIteratorWrapper<MultiIterator, MultiMatch>;

MatchGeneratorWrapper finditer_wrapper(Query& query, const std::string& doc) {
  const auto match_generator = query.finditer(doc);
  Iterator it = match_generator.begin();
  return MatchGeneratorWrapper(std::move(it));
}

MatchGeneratorWrapper finditer_stream_wrapper(Query& query, Reader* doc) {
  const auto match_generator = query.finditer(doc);
  Iterator it = match_generator.begin();
  return MatchGeneratorWrapper(std::move(it));
}

MultiMatchGeneratorWrapper multi_finditer_wrapper(MultiQuery& multi_query, const std::string& doc) {
  const auto multi_match_generator = multi_query.finditer(doc);
  auto it = multi_match_generator.begin();
  return MultiMatchGeneratorWrapper(std::move(it));
}

MultiMatchGeneratorWrapper multi_finditer_stream_wrapper(MultiQuery& multi_query, Reader* doc) {
  const auto multi_match_generator = multi_query.finditer(doc);
  auto it = multi_match_generator.begin();
  return MultiMatchGeneratorWrapper(std::move(it));
}

struct ReaderWrapper {
  explicit ReaderWrapper(const std::string& path) {
    stream.open(path, std::ios::in | std::ios::binary);
    if (!stream.is_open()) {
      throw REmatchException("Could not open file " + path);
    }
    reader = std::make_unique<FStreamReader>(stream);
  }
  std::fstream stream;
  std::unique_ptr<FStreamReader> reader;
};

// TODO: Fix bindings for emscripten (MatchGenerator & MultiMatchGenerator)
EMSCRIPTEN_BINDINGS(REmatchModule) {
  emscripten::value_array<Span>("Span").element(&Span::first).element(&Span::second);

  register_vector<std::string>("cppVectorString");
  register_vector<Span>("cppVectorSpan");
  register_vector<Match>("cppVectorMatch");

  emscripten::constant("DEFAULT_MAX_MEMPOOL_DUPLICATIONS", DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  emscripten::constant("DEFAULT_MAX_DETERMINISTIC_STATES", DEFAULT_MAX_DETERMINISTIC_STATES);
  emscripten::constant("DEFAULT_STREAM_BUFFER_SIZE", DEFAULT_STREAM_BUFFER_SIZE);

  enum_<Flags>("cppFlags").value("NONE", Flags::NONE).value("LINE_BY_LINE", Flags::LINE_BY_LINE);

  class_<ReaderWrapper>("cppReaderWrapper").constructor<const std::string&>();

  class_<Match>("cppMatch")
      .function("startIndex", select_overload<int64_t(uint_fast32_t) const>(&Match::start))
      .function("startVar", select_overload<int64_t(const std::string&) const>(&Match::start))
      .function("endIndex", select_overload<int64_t(uint_fast32_t) const>(&Match::end))
      .function("endVar", select_overload<int64_t(const std::string&) const>(&Match::end))
      .function("spanIndex", select_overload<Span(uint_fast32_t) const>(&Match::span))
      .function("spanVar", select_overload<Span(const std::string&) const>(&Match::span))
      .function("groupIndex", select_overload<std::string(uint_fast32_t) const>(&Match::group))
      .function("groupVar", select_overload<std::string(const std::string&) const>(&Match::group))
      .function("variables", &Match::variables)
      .function("empty", &Match::empty)
      .function("toString", &Match::to_string);

  class_<MatchGenerator>("cppMatchGenerator");

  class_<Query>("cppQuery")
      .function("findone", select_overload<Match(const std::string&) const>(&Query::findone))
      .function("findoneStream",
                std::function<Match(Query&, ReaderWrapper&)>(
                    [](Query& self, ReaderWrapper& r) { return self.findone(r.reader.get()); }))
      .function("findmany",
                select_overload<std::vector<Match>(const std::string&, uint_fast32_t) const>(
                    &Query::findmany))
      .function("findmanyStream",
                std::function<std::vector<Match>(Query&, ReaderWrapper&, uint_fast32_t)>(
                    [](Query& self, ReaderWrapper& r, uint_fast32_t limit) {
                      return self.findmany(r.reader.get(), limit);
                    }))
      .function("findall",
                select_overload<std::vector<Match>(const std::string&) const>(&Query::findall))
      .function("findallStream",
                std::function<std::vector<Match>(Query&, ReaderWrapper&)>(
                    [](Query& self, ReaderWrapper& r) { return self.findall(r.reader.get()); }))
      .function("finditer", &finditer_wrapper)
      .function("finditerStream", std::function<MatchGeneratorWrapper(Query&, ReaderWrapper&)>(
                                      [](Query& self, ReaderWrapper& r) {
                                        return finditer_stream_wrapper(self, r.reader.get());
                                      }))
      .function("check", select_overload<bool(const std::string&) const>(&Query::check))
      .function("checkStream",
                std::function<bool(Query&, ReaderWrapper&)>(
                    [](Query& self, ReaderWrapper& r) { return self.check(r.reader.get()); }))
      .function("variables", &Query::variables);

  class_<MultiMatch>("cppMultiMatch")
      .function("spansIndex",
                select_overload<std::vector<Span>(uint_fast32_t) const>(&MultiMatch::spans))
      .function("spansVar",
                select_overload<std::vector<Span>(const std::string&) const>(&MultiMatch::spans))
      .function("groupsIndex",
                select_overload<std::vector<std::string>(uint_fast32_t) const>(&MultiMatch::groups))
      .function("groupsVar", select_overload<std::vector<std::string>(const std::string&) const>(
                                 &MultiMatch::groups))
      .function("submatch", &MultiMatch::submatch)
      .function("empty", &MultiMatch::empty)
      .function("variables", &MultiMatch::variables)
      .function("toString", &MultiMatch::to_string);

  class_<MultiQuery>("cppMultiQuery")
      .function("findone",
                select_overload<MultiMatch(const std::string&) const>(&MultiQuery::findone))
      .function("findoneStream", std::function<MultiMatch(MultiQuery&, ReaderWrapper&)>(
                                     [](MultiQuery& self, ReaderWrapper& r) {
                                       return self.findone(r.reader.get());
                                     }))
      .function("findmany",
                select_overload<std::vector<MultiMatch>(const std::string&, uint_fast32_t) const>(
                    &MultiQuery::findmany))
      .function("findmanyStream",
                std::function<std::vector<MultiMatch>(MultiQuery&, ReaderWrapper&, uint_fast32_t)>(
                    [](MultiQuery& self, ReaderWrapper& r, uint_fast32_t limit) {
                      return self.findmany(r.reader.get(), limit);
                    }))
      .function("findall", select_overload<std::vector<MultiMatch>(const std::string&) const>(
                               &MultiQuery::findall))
      .function(
          "findallStream",
          std::function<std::vector<MultiMatch>(MultiQuery&, ReaderWrapper&)>(
              [](MultiQuery& self, ReaderWrapper& r) { return self.findall(r.reader.get()); }))
      .function("finditer", &multi_finditer_wrapper)
      .function("finditerStream",
                std::function<MultiMatchGeneratorWrapper(MultiQuery&, ReaderWrapper&)>(
                    [](MultiQuery& self, ReaderWrapper& r) {
                      return multi_finditer_stream_wrapper(self, r.reader.get());
                    }))
      .function("check", select_overload<bool(const std::string&) const>(&MultiQuery::check))
      .function("checkStream",
                std::function<bool(MultiQuery&, ReaderWrapper&)>(
                    [](MultiQuery& self, ReaderWrapper& r) { return self.check(r.reader.get()); }))
      .function("variables", &MultiQuery::variables);

  function("cppReql", &reql);
  function("cppMultiReql", &multi_reql);

  class_<MatchGeneratorWrapper>("cppMatchGeneratorIterator")
      .function("get", &MatchGeneratorWrapper::get)
      .function("hasValue", &MatchGeneratorWrapper::hasValue)
      .function("next", &MatchGeneratorWrapper::next);

  class_<MultiMatchGeneratorWrapper>("cppMultiMatchGeneratorIterator")
      .function("get", &MultiMatchGeneratorWrapper::get)
      .function("hasValue", &MultiMatchGeneratorWrapper::hasValue)
      .function("next", &MultiMatchGeneratorWrapper::next);
};
