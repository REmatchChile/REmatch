#pragma once

#include <iostream>
#include <map>

#include "output_enumeration/mapping.hpp"

#include <REmatch/span.hpp>

namespace REmatch::mediator {
class Mapping {
 public:
  Mapping() = default;

  explicit Mapping(std::map<int, Span>&& spans_map);

  Span get_span_of_variable(int variable_id) const;
  void add_span(int variable_id, Span span);
  void shift(int64_t shift);
  void reset();
  std::map<int, Span> get_spans_map() const;
  bool operator==(const Mapping& other) const;
  friend std::ostream& operator<<(std::ostream& os, Mapping const& mapping);

 private:
  std::map<int, Span> spans_map_ = {};
};
}  // namespace REmatch::mediator
