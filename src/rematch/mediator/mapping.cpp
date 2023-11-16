#include "mapping.hpp"

namespace rematch {
namespace mediator {

Mapping::Mapping(std::map<std::string, Span> spans_map) {
  spans_map_ = spans_map;
}

Span Mapping::get_span_of_variable(std::string variable_name) {
  if (spans_map_.count(variable_name)) {
    return spans_map_[variable_name];
  }

  throw REMatch::VariableNotFoundException(variable_name);
}

void Mapping::add_span(std::string variable_name, Span span) {
  spans_map_[variable_name] = span;
}

void Mapping::shift_spans(int shift) {
  for (auto& span_pair : spans_map_) {
    Span& span = span_pair.second;
    span.first += shift;
    span.second += shift;
  }
}

void Mapping::reset() {
  spans_map_.clear();
}

std::map<std::string, Span> Mapping::get_spans_map() {
  return spans_map_;
}

bool Mapping::operator==(const Mapping& other) const {
  return this->spans_map_ == other.spans_map_;
}

std::ostream& operator<<(std::ostream& os, Mapping const &mapping) {
  for (auto& pair : mapping.spans_map_) {
    Span span = pair.second;
    os << "  " << pair.first << " -> [" << span.first << ", " << span.second << "]\n";
  }
  return os;
}

}
}
