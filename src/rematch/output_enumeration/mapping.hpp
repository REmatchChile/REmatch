#ifndef OUTPUT_ENUMERATOR__MAPPING_HPP
#define OUTPUT_ENUMERATOR__MAPPING_HPP

#include <map>

#include "REmatch/span.hpp"
#include "output_enumeration/ecs.hpp"

namespace REmatch {

class ExtendedMapping;

inline namespace output_enumeration {

/**
 * A Mapping is also known as an s-tuple, asigns a span to all it's
 * variables. This is an internal mapping, therefore the spans are
 * assigned to the variable id's, to get the original variable names the
 * VariableCatalog class should be used.
 */
class Mapping {
 public:

  static constexpr int VARIABLE_MARKERS_SIZE = 64;

  struct Annotation {
    std::bitset<64> variable_markers;
    int64_t document_position;

    Annotation(std::bitset<64> variable_markers, int64_t document_position)
        : variable_markers(variable_markers), document_position(document_position) {}

    bool operator==(const Annotation& other) const {
      return document_position == other.document_position
            && variable_markers == other.variable_markers;
    }
    bool operator<(const Annotation& other) const {
      return document_position < other.document_position;
    }
  };

  Mapping();

  void add_annotations(std::bitset<64> variable_markers,
                       int64_t document_position);

  /**
   * assumptions: inverted_annotations are sorted by document_position,
   * and are functional.
   */
  std::vector<Span> get_spans_of_variable_id(int id) const;
  std::map<int, std::vector<Span>> construct_mapping() const;

  void delete_previous_annotation();
  void delete_all_annotations();

 private:
  static void process_annotation(const Annotation& annotation,
                          std::map<int, std::vector<Span>>& spans_map);
  static void add_span(std::map<int, std::vector<Span>>& spans_map,
                     int variable_id, int64_t document_position);
  static void update_last_span(std::map<int, std::vector<Span>>& spans_map,
                            int variable_id, int64_t document_position);

  Span get_next_span(int variable_id, int& next_possible_opening_position,
                     int& next_possible_closure_position) const;
  int get_next_variable_opening(int variable_id, int& current_position) const;
  int get_next_variable_closure(int variable_id, int& current_position) const;
  int find_next_document_position_where_the_specified_marker_is_true(
      int marker_id, int& current_position) const;

  std::vector<Annotation> inverted_annotations;

  friend class REmatch::ExtendedMapping;
};
}
}

#endif
