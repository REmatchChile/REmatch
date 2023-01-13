#ifndef OUTPUT_ENUMERATOR__MAPPING_HPP
#define OUTPUT_ENUMERATOR__MAPPING_HPP

#include "output_enumeration/ecs.hpp"

namespace rematch {

using Span = std::pair<int64_t, int64_t>;

class Mapping {

 const int NO_ANNOTATION = -1;

 private:
  std::vector<size_t> annotations;

 public:
  Mapping(int amount_of_variables);

  Span get_span_of_variable_id(int id) const;
  void add_annotations(std::bitset<64> variable_markers,
                       size_t document_position);

 private:
};  // end class Mapping

}  // end namespace rematch

#endif  // OUTPUT_ENUMERATOR__MAPPING_HPP
