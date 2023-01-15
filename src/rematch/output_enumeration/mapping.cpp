#include "output_enumeration/mapping.hpp"

namespace rematch {

Mapping::Mapping(int amount_of_variables) : 
    annotations() {}

void Mapping::add_annotations(std::bitset<64> variable_markers,
                     size_t document_position) {
  annotations.push_back(Annotation{variable_markers, document_position});
}

std::vector<Span> Mapping::get_spans_of_variable_id(int variable_id) const {
  /**
   * assumptions: annotations are sorted by document_position, and
   * are functional.
   *
   * The current implementation is not optimal, as it traverses the
   * annotations vector twice. Furthermore, if the spans of all the 
   * variables are found in a single pass it would be more efficient.
   */

  std::vector<Span> spans;
  int next_possible_opening_position = 0, next_possible_closure_position = 0;
  while (next_possible_opening_position < (int) annotations.size() &&
         next_possible_closure_position < (int) annotations.size()) {
    Span span = get_next_span(
        variable_id,
        next_possible_opening_position,
        next_possible_closure_position
    );
    if (span.first != -1 && span.second != -1) {
      spans.push_back(span);
    }
  }
  return spans;
}

void Mapping::delete_previous_annotation() {
  annotations.pop_back();
}

Span Mapping::get_next_span(int variable_id,
                   int &next_possible_opening_position,
                   int &next_possible_closure_position) const {
  Span span;
  span.first = get_next_variable_oppening(variable_id,
                                          next_possible_opening_position);
  span.second = get_next_variable_closure(variable_id,
                                          next_possible_closure_position);
  return span;
}

int Mapping::get_next_variable_oppening(int variable_id,
                                        int &current_position) const {
  return find_next_document_position_where_the_specified_marker_is_true(
      variable_id * 2, current_position
  );
}

int Mapping::get_next_variable_closure(int variable_id,
                                        int &current_position) const {
  int position_closed = 
    find_next_document_position_where_the_specified_marker_is_true(
      variable_id * 2 + 1, current_position
  );
  return position_closed;
}

int Mapping::find_next_document_position_where_the_specified_marker_is_true(
    int marker_id, int &current_position
) const {
  while (current_position < (int) annotations.size()) {
    if (annotations[current_position].variable_markers[marker_id]) {
      return annotations[current_position++].document_position;
    }
  }
  return -1;
}

}  // end namespace rematch
