#include <cstdint>

#include <emscripten/bind.h>


#include <REmatch/REmatch.hpp>

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

template class EmscriptenIteratorWrapper<MatchGenerator::iterator, Match>;
template class EmscriptenIteratorWrapper<MultiMatchGenerator::iterator,
                                         MultiMatch>;

using MatchGeneratorWrapper =
    EmscriptenIteratorWrapper<MatchGenerator::iterator, Match>;
using MultiMatchGeneratorWrapper =
    EmscriptenIteratorWrapper<MultiMatchGenerator::iterator, MultiMatch>;

MatchGeneratorWrapper finditer_wrapper(Query& query, const std::string& doc) {
  const auto match_generator = query.finditer(doc);
  auto it = match_generator.begin();
  return MatchGeneratorWrapper(std::move(it));
}

MultiMatchGeneratorWrapper multi_finditer_wrapper(MultiQuery& multi_query,
                                                  const std::string& doc) {
  const auto multi_match_generator = multi_query.finditer(doc);
  auto it = multi_match_generator.begin();
  return MultiMatchGeneratorWrapper(std::move(it));
}

// TODO: Fix bindings for emscripten (MatchGenerator & MultiMatchGenerator)
EMSCRIPTEN_BINDINGS(REmatchModule) {
  emscripten::value_array<Span>("Span")
      .element(&Span::first)
      .element(&Span::second);

  register_vector<std::string>("cppVectorString");
  register_vector<Span>("cppVectorSpan");
  register_vector<Match>("cppVectorMatch");

  emscripten::constant("DEFAULT_MAX_MEMPOOL_DUPLICATIONS",
                       DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  emscripten::constant("DEFAULT_MAX_DETERMINISTIC_STATES",
                       DEFAULT_MAX_DETERMINISTIC_STATES);

  enum_<Flags>("cppFlags")
      .value("NONE", Flags::NONE)
      .value("LINE_BY_LINE", Flags::LINE_BY_LINE);

  class_<Match>("cppMatch")
      .function("startIndex",
                select_overload<int64_t(uint_fast32_t) const>(&Match::start))
      .function("startVar", select_overload<int64_t(const std::string&) const>(
                                &Match::start))
      .function("endIndex",
                select_overload<int64_t(uint_fast32_t) const>(&Match::end))
      .function("endVar",
                select_overload<int64_t(const std::string&) const>(&Match::end))
      .function("spanIndex",
                select_overload<Span(uint_fast32_t) const>(&Match::span))
      .function("spanVar",
                select_overload<Span(const std::string&) const>(&Match::span))
      .function("groupIndex", select_overload<std::string(uint_fast32_t) const>(
                                  &Match::group))
      .function(
          "groupVar",
          select_overload<std::string(const std::string&) const>(&Match::group))
      .function("variables", &Match::variables)
      .function("empty", &Match::empty)
      .function("toString", &Match::to_string);

  class_<MatchGenerator>("cppMatchGenerator");

  class_<Query>("cppQuery")
      .function("findone", &Query::findone)
      .function("findmany", &Query::findmany)
      .function("findall", &Query::findall)
      .function("finditer", &finditer_wrapper)
      .function("check", &Query::check)
      .function("variables", &Query::variables);

  class_<MultiMatch>("cppMultiMatch")
      .function("spansIndex",
                select_overload<std::vector<Span>(uint_fast32_t) const>(
                    &MultiMatch::spans))
      .function("spansVar",
                select_overload<std::vector<Span>(const std::string&) const>(
                    &MultiMatch::spans))
      .function("groupsIndex",
                select_overload<std::vector<std::string>(uint_fast32_t) const>(
                    &MultiMatch::groups))
      .function(
          "groupsVar",
          select_overload<std::vector<std::string>(const std::string&) const>(
              &MultiMatch::groups))
      .function("submatch", &MultiMatch::submatch)
      .function("empty", &MultiMatch::empty)
      .function("variables", &MultiMatch::variables)
      .function("toString", &MultiMatch::to_string);

  class_<MultiQuery>("cppMultiQuery")
      .function("findone", &MultiQuery::findone)
      .function("findmany", &MultiQuery::findmany)
      .function("findall", &MultiQuery::findall)
      .function("finditer", &multi_finditer_wrapper)
      .function("check", &MultiQuery::check)
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
