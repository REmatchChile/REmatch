#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <iostream>
#include <string>
#include <map>
#include "output_enumeration/mapping.hpp"
#include "exceptions/variable_not_found_exception.hpp"

namespace rematch {
namespace mediator {

class Mapping {
 public:
  Mapping() = default;
  Mapping(std::map<std::string, Span> spans_map);
  Span get_span_of_variable(std::string variable_name);
  void add_span(std::string variable_name, Span span);
  void shift_spans(int shift);
  void reset();
  std::map<std::string, Span> get_spans_map();
  bool operator==(const Mapping& other) const;
  friend std::ostream& operator<<(std::ostream& os, Mapping const &mapping);

 private:
  std::map<std::string, Span> spans_map_ = {};
};
}
}

#endif
