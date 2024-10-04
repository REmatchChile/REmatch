#include "output_enumeration/mapping.hpp"

namespace REmatch {
inline namespace output_enumeration {

inline bool is_close_code(int bitset_index);

Mapping::Mapping() :
    inverted_annotations() {}

void Mapping::add_annotations(std::bitset<64> variable_markers,
                     size_t document_position) {
  inverted_annotations.push_back(Annotation{variable_markers, document_position});
}

void Mapping::delete_all_annotations() {
  inverted_annotations.clear();
}

std::map<int, std::vector<Span>> Mapping::construct_mapping() const {
  std::map<int, std::vector<Span>> spans_map;

  for (auto& annotation : inverted_annotations) {
    process_annotation(annotation, spans_map);
  }

  return spans_map;
}

void Mapping::process_annotation(const Annotation& annotation,
                                 std::map<int, std::vector<Span>>& spans_map) const {

  for (size_t bitset_index = 0;
       bitset_index < annotation.variable_markers.size() - 2; bitset_index++) {

    if (annotation.variable_markers[bitset_index]) {
      int variable_id = bitset_index / 2;

      if (is_close_code(bitset_index)) {
        add_span(spans_map, variable_id, annotation.document_position);
      } else {
        update_last_span(spans_map, variable_id,
                             annotation.document_position);
      }
    }
  }
}

bool is_close_code(int bitset_index) {
  return bitset_index % 2 == 1;
}

void Mapping::add_span(std::map<int, std::vector<Span>>& spans_map,
                            int variable_id, int document_position) const {
  spans_map[variable_id].push_back({0, document_position});
}

void Mapping::update_last_span(std::map<int, std::vector<Span>>& spans_map,
                                   int variable_id, int document_position) const {
  spans_map[variable_id].back().first = document_position;
}

std::vector<Span> Mapping::get_spans_of_variable_id(int variable_id) const {
   /**
    * The current implementation is not optimal, as it traverses the
    * inverted_annotations vector twice. Furthermore, if the spans of all
    * the variables are found in a single pass it would be more efficient.
    */
  std::vector<Span> spans;
  int next_possible_opening_position = 0, next_possible_closure_position = 0;
  while (next_possible_opening_position < (int) inverted_annotations.size() &&
         next_possible_closure_position < (int) inverted_annotations.size()) {
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
  inverted_annotations.pop_back();
  // TODO: change this so that it goes back to the last
  // annotation that was added
}

// TODO: Change logic to have less function calls or make it cleaner.
Span Mapping::get_next_span(int variable_id,
                   int &next_possible_opening_position,
                   int &next_possible_closure_position) const {
  /**
   * The logic behind this implementation is storing the last opening and
   * the last closure that were found, therefore the variable annotations
   * vector is traversed twice. The reason why this implementation is used,
   * is because it becomes too cumbersome when analysing border cases where
   * one variable is oppened and closed at the same moment.
   */
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
  while (current_position < (int) inverted_annotations.size()) {
    if (inverted_annotations[current_position].variable_markers[marker_id]) {
      return inverted_annotations[current_position++].document_position;
    }
    current_position++;
  }
  return -1;
}

}  // namespace output_enumeration
}
