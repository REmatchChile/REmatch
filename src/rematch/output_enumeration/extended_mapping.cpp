#include "output_enumeration/extended_mapping.hpp"
#include <algorithm>

namespace rematch {

ExtendedMapping::ExtendedMapping(const output_enumeration::Mapping& mapping)
    : inverted_annotations_(mapping.inverted_annotations) {}

ExtendedMapping::ExtendedMapping(std::vector<Mapping::Annotation> annotations)
    : inverted_annotations_(annotations) {}

std::map<int, std::vector<Span>> ExtendedMapping::construct_mapping() const {
  std::map<int, std::vector<Span>> spans_map;
  std::map<int, Span> spans_buffer;

  // iterate in reverse so that the positions are in ascending order
  for (auto it = inverted_annotations_.rbegin();
       it != inverted_annotations_.rend(); ++it) {
    process_annotation(*it, spans_map, spans_buffer);
  }

  return spans_map;
}

void ExtendedMapping::process_annotation(
    const Mapping::Annotation& annotation,
    std::map<int, std::vector<Span>>& spans_map,
    std::map<int, Span>& spans_buffer) const {

  // Iterate starting from the last index to process the closed variable first
  // in case the variable is opened and closed in the same position.
  // -3 because we skip the first 2 bits since they are used to label the ecs nodes
  for (int bitset_index = annotation.variable_markers.size() - 3;
       bitset_index >= 0; --bitset_index) {
    if (annotation.variable_markers[bitset_index]) {
      int variable_id = bitset_index / 2;

      if (is_open_code(bitset_index)) {
        add_span_with_opened_position(variable_id, annotation.document_position,
                                      spans_buffer);
      } else {
        update_last_span_with_closed_position(
            spans_map, variable_id, annotation.document_position, spans_buffer);
      }
    }
  }
}

inline bool ExtendedMapping::is_open_code(int bitset_index) const {
  return bitset_index % 2 == 0;
}

void ExtendedMapping::add_span_with_opened_position(
    int variable_id, int document_position,
    std::map<int, Span>& spans_buffer) const {
  spans_buffer[variable_id].first = document_position;
  spans_buffer[variable_id].second = INVALID_POSITION;
}

void ExtendedMapping::update_last_span_with_closed_position(
    std::map<int, std::vector<Span>>& spans_map, int variable_id,
    int document_position, std::map<int, Span>& spans_buffer) const {
  if (spans_buffer[variable_id].second == INVALID_POSITION) {
    spans_buffer[variable_id].second = document_position;
    spans_map[variable_id].push_back(spans_buffer[variable_id]);
  }
}

ExtendedMapping ExtendedMapping::get_submapping(Span span) {
  auto annotations_rbegin = std::lower_bound(
      inverted_annotations_.rbegin(), inverted_annotations_.rend(),
      Mapping::Annotation{{}, (size_t)span.first});

  auto annotations_rend = std::upper_bound(
      inverted_annotations_.rbegin(), inverted_annotations_.rend(),
      Mapping::Annotation{{}, (size_t)span.second});

  return std::vector<Mapping::Annotation>(annotations_rend.base(),
                                          annotations_rbegin.base());
}

void ExtendedMapping::shift_positions(int shift) {
  for (auto& annotation : inverted_annotations_) {
    annotation.document_position += shift;
  }
}

bool ExtendedMapping::operator==(const ExtendedMapping& other) const {
  return this->inverted_annotations_ == other.inverted_annotations_;
}

std::ostream& operator<<(std::ostream& os, ExtendedMapping& extended_mapping) {
  for (auto& annotation : extended_mapping.inverted_annotations_) {
    os << annotation.variable_markers << " : " << annotation.document_position
       << "\n";
  }
  return os;
}

}  // namespace rematch
