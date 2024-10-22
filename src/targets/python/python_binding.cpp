#include <pybind11/attr.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <REmatch/REmatch.hpp>

namespace py = pybind11;
using namespace REmatch;
using namespace pybind11::literals;

PYBIND11_MODULE(_pyrematch, m) {
  m.doc() = "REmatch";

  m.attr("DEFAULT_MAX_MEMPOOL_DUPLICATIONS") =
      py::int_(DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  m.attr("DEFAULT_MAX_DETERMINISTIC_STATES") =
      py::int_(DEFAULT_MAX_DETERMINISTIC_STATES);

  py::enum_<Flags>(m, "cppFlags", py::arithmetic())
      .value("NONE", Flags::NONE)
      .value("LINE_BY_LINE", Flags::LINE_BY_LINE);

  py::class_<Match>(m, "cppMatch")
      .def("start",
           py::overload_cast<const std::string&>(&Match::start, py::const_))
      .def("start", py::overload_cast<uint_fast32_t>(&Match::start, py::const_))
      .def("end",
           py::overload_cast<const std::string&>(&Match::end, py::const_))
      .def("end", py::overload_cast<uint_fast32_t>(&Match::end, py::const_))
      .def("span",
           py::overload_cast<const std::string&>(&Match::span, py::const_))
      .def("span", py::overload_cast<uint_fast32_t>(&Match::span, py::const_))
      .def("group",
           py::overload_cast<const std::string&>(&Match::group, py::const_))
      .def("group", py::overload_cast<uint_fast32_t>(&Match::group, py::const_))
      .def("variables", &Match::variables)
      .def("empty", &Match::empty)
      .def("to_string", &Match::to_string);

  py::class_<MatchGenerator::iterator>(m, "cppMatchGeneratorIterator")
      .def("get", [](MatchGenerator::iterator& self) -> Match { return *self; })
      .def("has_value",
           [](MatchGenerator::iterator& self) -> bool {
             return self.operator->() != nullptr;
           })
      .def("next", [](MatchGenerator::iterator& self) -> void { ++self; });

  py::class_<MatchGenerator>(m, "cppMatchGenerator")
      .def("begin", &MatchGenerator::begin);

  py::class_<Query>(m, "cppQuery")
      .def("findone", &Query::findone)
      .def("findmany", &Query::findmany)
      .def("findall", &Query::findall)
      .def("finditer", &Query::finditer)
      .def("check", &Query::check)
      .def("variables", &Query::variables);

  py::class_<MultiMatch>(m, "cppMultiMatch")
      .def("spans", py::overload_cast<const std::string&>(&MultiMatch::spans,
                                                          py::const_))
      .def("spans",
           py::overload_cast<uint_fast32_t>(&MultiMatch::spans, py::const_))
      .def("groups", py::overload_cast<const std::string&>(&MultiMatch::groups,
                                                           py::const_))
      .def("groups",
           py::overload_cast<uint_fast32_t>(&MultiMatch::groups, py::const_))
      .def("submatch", &MultiMatch::submatch)
      .def("empty", &MultiMatch::empty)
      .def("variables", &MultiMatch::variables)
      .def("to_string", &MultiMatch::to_string);

  py::class_<MultiMatchGenerator::iterator>(m, "cppMultiMatchGeneratorIterator")
      .def("get",
           [](MultiMatchGenerator::iterator& self) -> MultiMatch {
             return *self;
           })
      .def("has_value",
           [](MultiMatchGenerator::iterator& self) -> bool {
             return self.operator->() != nullptr;
           })
      .def("next", [](MultiMatchGenerator::iterator& self) -> void { ++self; });

  py::class_<MultiMatchGenerator>(m, "cppMultiMatchGenerator")
      .def("begin", &MultiMatchGenerator::begin);

  py::class_<MultiQuery>(m, "cppMultiQuery")
      .def("findone", &MultiQuery::findone)
      .def("findmany", &MultiQuery::findmany)
      .def("findall", &MultiQuery::findall)
      .def("finditer", &MultiQuery::finditer)
      .def("check", &MultiQuery::check)
      .def("variables", &MultiQuery::variables);

  m.def("cppreql", &reql, "pattern"_a, "flags"_a = Flags::NONE,
        "max_mempool_duplications"_a = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
        "max_deterministic_states"_a = DEFAULT_MAX_DETERMINISTIC_STATES);

  m.def("cppmulti_reql", &multi_reql, "pattern"_a, "flags"_a = Flags::NONE,
        "max_mempool_duplications"_a = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
        "max_deterministic_states"_a = DEFAULT_MAX_DETERMINISTIC_STATES);

  // TODO: In a future we would like to register all exceptions and keep the inheritance
  py::register_exception<REmatchException>(m, "REmatchException");
  //   py::register_exception<QuerySyntaxException>(m, "QuerySyntaxException");
  //   py::register_exception<AnchorInsideCaptureException>(
  //       m, "AnchorInsideCaptureException");
  //   py::register_exception<ComplexQueryException>(m, "ComplexQueryException");
  //   py::register_exception<EmptyWordCaptureException>(
  //       m, "EmptyWordCaptureException");
  //   py::register_exception<InvalidCharacterException>(
  //       m, "InvalidCharacterException");
  //   py::register_exception<InvalidEscapeException>(m, "InvalidEscapeException");
  //   py::register_exception<InvalidRangeException>(m, "InvalidRangeException");
  //   py::register_exception<MemoryLimitExceededException>(
  //       m, "MemoryLimitExceededException");
  //   py::register_exception<SameNestedVariableException>(
  //       m, "SameNestedVariableException");
  //   py::register_exception<UnhandledExpressionException>(
  //       m, "UnhandledExpressionException");
  //   py::register_exception<VariableLimitExceededException>(
  //       m, "VariableLimitExceededException");
  //   py::register_exception<VariableNotFoundException>(
  //       m, "VariableNotFoundException");
  //   py::register_exception<VariableNotFoundInCatalogException>(
  //       m, "VariableNotFoundInCatalogException");
  //   py::register_exception<MultiSpannersNotAllowedException>(
  //       m, "MultiSpannersNotAllowedException");
}
