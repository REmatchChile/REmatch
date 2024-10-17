#pragma once

#include <iostream>
#include <string>
#include <map>
#include "output_enumeration/mapping.hpp"

#include <REmatch/span.hpp>

namespace REmatch {
namespace mediator {

class Mapping {
 public:
  Mapping() = default;
  Mapping(std::map<std::string, Span>&& spans_map);

  Span get_span_of_variable(const std::string& variable_name) const;
  void add_span(std::string variable_name, Span span);
  void shift_spans(int shift);
  void reset();
  std::map<std::string, Span> get_spans_map() const;
  bool operator==(const Mapping& other) const;
  friend std::ostream& operator<<(std::ostream& os, Mapping const &mapping);

 private:
  std::map<std::string, Span> spans_map_ = {};
};
}
}
