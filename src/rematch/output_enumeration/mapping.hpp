#ifndef OUTPUT_ENUMERATOR__MAPPING_HPP
#define OUTPUT_ENUMERATOR__MAPPING_HPP

#include "output_enumeration/ecs.hpp"

namespace rematch {

inline namespace output_enumeration {

using Span = std::pair<int64_t, int64_t>;

class Mapping {

 private:

  struct Annotation {
    std::bitset<64> variable_markers;
    size_t document_position;
  };
  std::vector<Annotation> annotations;

 public:
  Mapping(int amount_of_variables);

  void add_annotations(std::bitset<64> variable_markers,
                       size_t document_position);
  std::vector<Span> get_spans_of_variable_id(int id) const;
  void delete_previous_annotation();

 private:
  Span get_next_span(int variable_id,
                     int &next_possible_opening_position,
                     int &next_possible_closure_position) const;
  int get_next_variable_oppening(int variable_id, int &current_position) const;
  int get_next_variable_closure(int variable_id, int &current_position) const;
  int find_next_document_position_where_the_specified_marker_is_true(
    int marker_id, int &current_position) const;

};  // end class Mapping

}  // end namespace output_enumeration

}  // end namespace rematch

#endif  // OUTPUT_ENUMERATOR__MAPPING_HPP
