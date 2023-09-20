#include "mapping.hpp"

namespace rematch {
namespace mediator {

Span Mapping::get_span_of_variable(std::string variable) {
  if (spans_map.count(variable)) {
    return spans_map[variable];
  }

  throw std::out_of_range("The variable is not valid.");
}

void Mapping::add_span(std::string variable_id, Span span) {
  spans_map[variable_id] = span;
}

void Mapping::shift_spans(int shift) {
  for (auto& span_pair : spans_map) {
    Span& span = span_pair.second;
    span.first += shift;
    span.second += shift;
  }
}

}
}
