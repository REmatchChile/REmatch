#include "mapping.hpp"

#include <REmatch/exceptions.hpp>

namespace REmatch::mediator {

Mapping::Mapping(std::map<int, Span>&& spans_map) {
  spans_map_ = std::move(spans_map);
}

Span Mapping::get_span_of_variable(int variable_id) const {
  if (spans_map_.count(variable_id)) {
    return spans_map_.at(variable_id);
  }

  throw REmatch::VariableNotFoundException(std::to_string(variable_id));
}

void Mapping::add_span(int variable_id, Span span) {
  spans_map_[variable_id] = span;
}

void Mapping::shift(int64_t shift) {
  for (auto& span_pair : spans_map_) {
    Span& span = span_pair.second;
    span.first += shift;
    span.second += shift;
  }
}

void Mapping::reset() {
  spans_map_.clear();
}

std::map<int, Span> Mapping::get_spans_map() const {
  return spans_map_;
}

bool Mapping::operator==(const Mapping& other) const {
  return this->spans_map_ == other.spans_map_;
}

std::ostream& operator<<(std::ostream& os, Mapping const& mapping) {
  for (auto& pair : mapping.spans_map_) {
    Span span = pair.second;
    os << "  " << pair.first << " -> [" << span.first << ", " << span.second << "]\n";
  }
  return os;
}

}  // namespace REmatch::mediator
