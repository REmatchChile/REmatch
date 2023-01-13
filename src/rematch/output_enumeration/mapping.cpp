#include "output_enumeration/mapping.hpp"

namespace rematch {

Mapping::Mapping(int amount_of_variables) : 
    annotations(amount_of_variables * 2, NO_ANNOTATION) {}

Span Mapping::get_span_of_variable_id(int id) const {
  return Span(annotations[id * 2], annotations[id * 2 + 1]);
}

void Mapping::add_annotations(std::bitset<64> variable_markers,
                     size_t document_position) {
  for (int i = 0; i < (int) annotations.size(); i++) {
    if (variable_markers[i]) {
      annotations[i] = document_position;
    }
  }
}

}  // end namespace rematch
