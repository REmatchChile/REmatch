#include <string>
#include <map>
#include "output_enumeration/mapping.hpp"

namespace rematch {
namespace mediator {

class Mapping {
 public:
  Span get_span_of_variable(std::string variable_id);
  void add_span(std::string variable_id, Span span);
  void shift_spans(int shift);

  std::map<std::string, Span> spans_map = {};
};
}
}
