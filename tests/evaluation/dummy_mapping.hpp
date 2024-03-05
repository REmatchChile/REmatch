#ifndef DUMMY_MAPPING_HPP
#define DUMMY_MAPPING_HPP

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "exceptions/variable_not_found_exception.hpp"

namespace rematch::testing {

struct DummyMapping {
  std::map<std::string, Span> spans_map;
  int number_of_variables;

  DummyMapping(const std::map<std::string, Span>& data) : spans_map(data) {
    number_of_variables = data.size();
  }

  void add_span(std::string variable_name, Span span) {
    spans_map[variable_name] = span;
  }

  Span span(std::string variable_name) {
    if (spans_map.count(variable_name))
      return spans_map[variable_name];

   throw REMatch::VariableNotFoundException(variable_name);
  }

  bool operator==(const DummyMapping& other) const {
    return spans_map == other.spans_map;
  }
};
}

#endif
