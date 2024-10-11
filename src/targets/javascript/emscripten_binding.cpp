#include <emscripten/bind.h>

#include <REmatch/REmatch.hpp>
#include <cstdint>
#include "REmatch/flags.hpp"

using namespace emscripten;
using namespace REmatch;

EMSCRIPTEN_BINDINGS(REmatchModule) {
  value_array<Span>("Span").element(&Span::first).element(&Span::second);

  register_vector<std::string>("VectorString");
  register_vector<Span>("VectorSpan");

  emscripten::constant("DEFAULT_MAX_MEMPOOL_DUPLICATIONS",
                       DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  emscripten::constant("DEFAULT_MAX_DETERMINISTIC_STATES",
                       DEFAULT_MAX_DETERMINISTIC_STATES);

  enum_<Flags>("cppFlags")
      .constructor<>()
      .property("NONE", Flags::NONE)
      .property("LINE_BY_LINE", Flags::LINE_BY_LINE);

  class_<Match>("cppMatch")
      .function("startIndex",
                select_overload<int64_t(uint_fast32_t)>(&Match::start))
      .function("startVar",
                select_overload<int64_t(const std::string&)>(&Match::start))
      .function("endIndex",
                select_overload<int64_t(uint_fast32_t)>(&Match::end))
      .function("endVar",
                select_overload<int64_t(const std::string&)>(&Match::end))
      .function("spanIndex", select_overload<Span(int)>(&Match::span))
      .function("spanVar",
                select_overload<Span(const std::string&)>(&Match::span))
      .function("groupIndex",
                select_overload<std::string(uint_fast32_t)>(&Match::group))
      .function("groupVar",
                select_overload<std::string(const std::string&)>(&Match::group))
      .function("variables", &Match::variables)
      .function("empty", &Match::empty)
      .function("toString", &Match::to_string);

  class_<MatchIterator>("cppMatchIterator")
      .function("next", &MatchIterator::next)
      .function("variables", &MatchIterator::variables);

  class_<Query>("cppQuery")
      .function("findone", &Query::findone)
      .function("findall", &Query::findall)
      .function("finditer", &Query::finditer)
      .function("check", &Query::check);

  class_<MultiMatch>("cppMultiMatch")
      .function("spansIndex", select_overload<std::vector<Span>(uint_fast32_t)>(
                                  &MultiMatch::spans))
      .function("spansVar", select_overload<std::vector<Span>(std::string)>(
                                &MultiMatch::spans))
      .function("groupsIndex",
                select_overload<std::vector<std::string>(uint_fast32_t)>(
                    &MultiMatch::groups))
      .function("groupsVar",
                select_overload<std::vector<std::string>(std::string)>(
                    &MultiMatch::groups))
      .function("submatch", &MultiMatch::submatch)
      .function("empty", &MultiMatch::empty)
      .function("variables", &MultiMatch::variables)
      .function("toString", &MultiMatch::to_string);

  class_<MultiMatchIterator>("cppMultiMatchIterator")
      .function("next", &MultiMatchIterator::next)
      .function("variables", &MultiMatchIterator::variables);

  class_<MultiQuery>("cppMultiQuery")
      .function("findone", &MultiQuery::findone)
      .function("findall", &MultiQuery::findall)
      .function("finditer", &MultiQuery::finditer)
      .function("check", &MultiQuery::check);

  function("cppreql", &reql);
  function("cppmulti_reql", &multi_reql);
};
