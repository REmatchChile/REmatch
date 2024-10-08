#include <emscripten/bind.h>

#include <REmatch/REmatch.hpp>

using namespace emscripten;
using namespace REmatch;

EMSCRIPTEN_BINDINGS(REmatchModule) {
  value_array<Span>("Span").element(&Span::first).element(&Span::second);

  register_vector<std::string>("VectorString");
  register_vector<Span>("VectorSpan");

  class_<Flags>("Flags")
      .constructor<>()
      .property("line_by_line", &Flags::line_by_line)
      .property("max_mempool_duplications", &Flags::max_mempool_duplications)
      .property("max_deterministic_states", &Flags::max_deterministic_states);

  class_<Match>("Match")
      .function("start_index", select_overload<int(int)>(&Match::start))
      .function("start_var", select_overload<int(std::string)>(&Match::start))
      .function("end_index", select_overload<int(int)>(&Match::end))
      .function("end_var", select_overload<int(std::string)>(&Match::end))
      .function("span_index", select_overload<Span(int)>(&Match::span))
      .function("span_var", select_overload<Span(std::string)>(&Match::span))
      .function("group_index", select_overload<std::string(int)>(&Match::group))
      .function("group_var",
                select_overload<std::string(std::string)>(&Match::group))
      .function("variables", &Match::variables)
      .function("empty", &Match::empty);

  class_<MatchIterator>("MatchIterator")
      .function("next", &MatchIterator::next)
      .function("variables", &MatchIterator::variables);

  class_<Query>("Query")
      .constructor<const std::string&, Flags>()
      .function("findone", &Query::findone)
      .function("finditer", &Query::finditer)
      .function("check", &Query::check);

  class_<MultiMatch>("MultiMatch")
      .function("spans_index",
                select_overload<std::vector<Span>(int)>(&MultiMatch::spans))
      .function("spans_var", select_overload<std::vector<Span>(std::string)>(
                                 &MultiMatch::spans))
      .function("groups_index", select_overload<std::vector<std::string>(int)>(
                                    &MultiMatch::groups))
      .function("groups_var",
                select_overload<std::vector<std::string>(std::string)>(
                    &MultiMatch::groups))
      .function("submatch", &MultiMatch::submatch)
      .function("empty", &MultiMatch::empty);

  class_<MultiMatchIterator>("MultiMatchIterator")
      .function("next", &MultiMatchIterator::next)
      .function("variables", &MultiMatchIterator::variables);

  class_<MultiQuery>("MultiQuery")
      .constructor<const std::string&, Flags>()
      .function("findone", &MultiQuery::findone)
      .function("finditer", &MultiQuery::finditer)
      .function("check", &MultiQuery::check);

  function("compile", &compile);
  function("findone", &findone);
  function("findall", &findall);
  function("finditer", &finditer);
};
