#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>

#include "exceptions/anchor_inside_capture_exception.hpp"
#include "exceptions/exceptions.hpp"
#include "library_interface/multi_query.hpp"
#include "library_interface/query.hpp"
#include "library_interface/rematch.hpp"

namespace py = pybind11;
using namespace REMatch;
using namespace pybind11::literals;

PYBIND11_MODULE(_pyrematch, m) {
  m.doc() = "REmatch";

  py::class_<Flags>(m, "PyFlags")
      .def(py::init<>())
      .def_readwrite("line_by_line", &Flags::line_by_line)
      .def_readwrite("early_output", &Flags::early_output)
      .def_readwrite("max_mempool_duplications",
                     &Flags::max_mempool_duplications)
      .def_readwrite("max_deterministic_states",
                     &Flags::max_deterministic_states);

  py::class_<Match>(m, "PyMatch")
      .def("start", py::overload_cast<int>(&Match::start))
      .def("start", py::overload_cast<std::string>(&Match::start))
      .def("end", py::overload_cast<int>(&Match::end))
      .def("end", py::overload_cast<std::string>(&Match::end))
      .def("span", py::overload_cast<int>(&Match::span))
      .def("span", py::overload_cast<std::string>(&Match::span))
      .def("group", py::overload_cast<int>(&Match::group))
      .def("group", py::overload_cast<std::string>(&Match::group))
      .def("variables", &Match::variables)
      .def("empty", &Match::empty);

  py::class_<MatchIterator>(m, "PyMatchIterator")
      .def("next", &MatchIterator::next)
      .def("variables", &MatchIterator::variables);

  py::class_<Query>(m, "PyQuery")
      .def(py::init<const std::string&, Flags>())
      .def("findone", &Query::findone, "document"_a)
      .def("finditer", &Query::finditer, "document"_a)
      .def("check", &Query::check, "document"_a);

  py::class_<MultiMatch>(m, "PyMultiMatch")
      .def("spans", py::overload_cast<int>(&MultiMatch::spans))
      .def("spans", py::overload_cast<std::string>(&MultiMatch::spans))
      .def("groups", py::overload_cast<int>(&MultiMatch::groups))
      .def("groups", py::overload_cast<std::string>(&MultiMatch::groups))
      .def("submatch", &MultiMatch::submatch)
      .def("empty", &MultiMatch::empty);

  py::class_<MultiMatchIterator>(m, "PyMultiMatchIterator")
      .def("next", &MultiMatchIterator::next)
      .def("variables", &MultiMatchIterator::variables);

  py::class_<MultiQuery>(m, "PyMultiQuery")
      .def(py::init<const std::string&, Flags>())
      .def("findone", &MultiQuery::findone, "document"_a)
      .def("finditer", &MultiQuery::finditer, "document"_a)
      .def("check", &MultiQuery::check, "document"_a);

  m.def("compile", &compile, "pattern"_a, "flags"_a = Flags());
  m.def("findone", &findone, "pattern"_a, "document"_a, "flags"_a = Flags());
  m.def("findall", &findall, "pattern"_a, "document"_a, "flags"_a = Flags());
  m.def("finditer", &finditer, "pattern"_a, "document"_a, "flags"_a = Flags());

  py::register_exception<QuerySyntaxException>(m, "QuerySyntaxException");
  py::register_exception<AnchorInsideCaptureException>(m, "AnchorInsideCaptureException");
  py::register_exception<ComplexQueryException>(m, "ComplexQueryException");
  py::register_exception<EmptyWordCaptureException>(m, "EmptyWordCaptureException");
  py::register_exception<InvalidCharacterException>(m, "InvalidCharacterException");
  py::register_exception<InvalidEscapeException>(m, "InvalidEscapeException");
  py::register_exception<InvalidRangeException>(m, "InvalidRangeException");
  py::register_exception<MemoryLimitExceededException>(m, "MemoryLimitExceededException");
  py::register_exception<SameNestedVariableException>(m, "SameNestedVariableException");
  py::register_exception<UnhandledExpressionException>(m, "UnhandledExpressionException");
  py::register_exception<VariableLimitExceededException>(m, "VariableLimitExceededException");
  py::register_exception<VariableNotFoundException>(m, "VariableNotFoundException");
  py::register_exception<VariableNotFoundInCatalogException>(m, "VariableNotFoundInCatalogException");
  py::register_exception<MultiSpannersNotAllowedException>(m, "MultiSpannersNotAllowedException");
}
