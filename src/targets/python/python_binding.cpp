#include <pybind11/attr.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <fstream>

#include "REmatch/REmatch.hpp"

namespace py = pybind11;
using namespace REmatch;
using namespace pybind11::literals;

struct ReaderWrapper {
  explicit ReaderWrapper(const std::string& path) {
    stream.open(path, std::ios::in | std::ios::binary);
    if (!stream) {
      throw REmatchException("Could not open file " + path);
    }
    reader = std::make_unique<FStreamReader>(stream);
  }
  std::fstream stream;
  std::unique_ptr<FStreamReader> reader;
};

PYBIND11_MODULE(_pyrematch, m) {
  m.doc() = "REmatch";

  m.attr("DEFAULT_MAX_MEMPOOL_DUPLICATIONS") = py::int_(DEFAULT_MAX_MEMPOOL_DUPLICATIONS);
  m.attr("DEFAULT_MAX_DETERMINISTIC_STATES") = py::int_(DEFAULT_MAX_DETERMINISTIC_STATES);
  m.attr("DEFAULT_STREAM_BUFFER_SIZE") = py::int_(DEFAULT_STREAM_BUFFER_SIZE);

  py::enum_<Flags>(m, "cppFlags", py::arithmetic())
      .value("NONE", Flags::NONE)
      .value("LINE_BY_LINE", Flags::LINE_BY_LINE)
      .def(py::self | py::self);

  py::class_<ReaderWrapper>(m, "cppReaderWrapper").def(py::init<const std::string&>());

  py::class_<Match>(m, "cppMatch")
      .def("start", py::overload_cast<const std::string&>(&Match::start, py::const_))
      .def("start", py::overload_cast<uint_fast32_t>(&Match::start, py::const_))
      .def("end", py::overload_cast<const std::string&>(&Match::end, py::const_))
      .def("end", py::overload_cast<uint_fast32_t>(&Match::end, py::const_))
      .def("span", py::overload_cast<const std::string&>(&Match::span, py::const_))
      .def("span", py::overload_cast<uint_fast32_t>(&Match::span, py::const_))
      .def("group", py::overload_cast<const std::string&>(&Match::group, py::const_))
      .def("group", py::overload_cast<uint_fast32_t>(&Match::group, py::const_))
      .def("variables", &Match::variables)
      .def("empty", &Match::empty)
      .def("to_string", &Match::to_string);

  py::class_<Iterator>(m, "cppIterator")
      .def("get", [](Iterator& self) -> Match { return *self; })
      .def("has_value", [](Iterator& self) -> bool { return self.operator->() != nullptr; })
      .def("next", [](Iterator& self) -> void { ++self; });

  py::class_<MatchGenerator>(m, "cppMatchGenerator").def("begin", &MatchGenerator::begin);

  py::class_<Query>(m, "cppQuery")
      .def("findone", py::overload_cast<const std::string&>(&Query::findone, py::const_))
      .def("findone", [](Query& self, ReaderWrapper& r) { return self.findone(r.reader.get()); })
      .def("findmany",
           py::overload_cast<const std::string&, uint_fast32_t>(&Query::findmany, py::const_))
      .def("findmany", [](Query& self, ReaderWrapper& r,
                          uint_fast32_t limit) { return self.findmany(r.reader.get(), limit); })
      .def("findall", py::overload_cast<const std::string&>(&Query::findall, py::const_))
      .def("findall", [](Query& self, ReaderWrapper& r) { return self.findall(r.reader.get()); })
      .def("finditer", py::overload_cast<const std::string&>(&Query::finditer, py::const_))
      .def("finditer", [](Query& self, ReaderWrapper& r) { return self.finditer(r.reader.get()); })
      .def("check", py::overload_cast<const std::string&>(&Query::check, py::const_))
      .def("check", [](Query& self, ReaderWrapper& r) { return self.check(r.reader.get()); })
      .def("variables", &Query::variables);

  py::class_<MultiMatch>(m, "cppMultiMatch")
      .def("spans", py::overload_cast<const std::string&>(&MultiMatch::spans, py::const_))
      .def("spans", py::overload_cast<uint_fast32_t>(&MultiMatch::spans, py::const_))
      .def("groups", py::overload_cast<const std::string&>(&MultiMatch::groups, py::const_))
      .def("groups", py::overload_cast<uint_fast32_t>(&MultiMatch::groups, py::const_))
      .def("submatch", &MultiMatch::submatch)
      .def("empty", &MultiMatch::empty)
      .def("variables", &MultiMatch::variables)
      .def("to_string", &MultiMatch::to_string);

  py::class_<MultiIterator>(m, "cppMultiIterator")
      .def("get", [](MultiIterator& self) -> MultiMatch { return *self; })
      .def("has_value", [](MultiIterator& self) -> bool { return self.operator->() != nullptr; })
      .def("next", [](MultiIterator& self) -> void { ++self; });

  py::class_<MultiMatchGenerator>(m, "cppMultiMatchGenerator")
      .def("begin", &MultiMatchGenerator::begin);

  py::class_<MultiQuery>(m, "cppMultiQuery")
      .def("findone", py::overload_cast<const std::string&>(&MultiQuery::findone, py::const_))
      .def("findone",
           [](MultiQuery& self, ReaderWrapper& r) { return self.findone(r.reader.get()); })
      .def("findmany",
           py::overload_cast<const std::string&, uint_fast32_t>(&MultiQuery::findmany, py::const_))
      .def("findmany", [](MultiQuery& self, ReaderWrapper& r,
                          uint_fast32_t limit) { return self.findmany(r.reader.get(), limit); })
      .def("findall", py::overload_cast<const std::string&>(&MultiQuery::findall, py::const_))
      .def("findall",
           [](MultiQuery& self, ReaderWrapper& r) { return self.findall(r.reader.get()); })
      .def("finditer", py::overload_cast<const std::string&>(&MultiQuery::finditer, py::const_))
      .def("finditer",
           [](MultiQuery& self, ReaderWrapper& r) { return self.finditer(r.reader.get()); })
      .def("check", py::overload_cast<const std::string&>(&MultiQuery::check, py::const_))
      .def("check", [](MultiQuery& self, ReaderWrapper& r) { return self.check(r.reader.get()); })
      .def("variables", &MultiQuery::variables);

  m.def("cppreql", &reql, "pattern"_a, "flags"_a = Flags::NONE,
        "max_mempool_duplications"_a = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
        "max_deterministic_states"_a = DEFAULT_MAX_DETERMINISTIC_STATES,
        "stream_buffer_size"_a = DEFAULT_STREAM_BUFFER_SIZE);

  m.def("cppmulti_reql", &multi_reql, "pattern"_a, "flags"_a = Flags::NONE,
        "max_mempool_duplications"_a = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
        "max_deterministic_states"_a = DEFAULT_MAX_DETERMINISTIC_STATES,
        "stream_buffer_size"_a = DEFAULT_STREAM_BUFFER_SIZE);

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
