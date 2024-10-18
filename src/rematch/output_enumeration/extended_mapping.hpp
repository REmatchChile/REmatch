#ifndef EXTENDED_MAPPING_HPP
#define EXTENDED_MAPPING_HPP

#include <optional>
#include "output_enumeration/mapping.hpp"

#include <REmatch/span.hpp>

namespace REmatch {

constexpr int INVALID_POSITION = -1;

class ExtendedMapping {
 public:
  ExtendedMapping(const output_enumeration::Mapping& mapping);
  ExtendedMapping(std::vector<Mapping::Annotation>&& annotations);

  std::map<int, std::vector<Span>> construct_mapping() const;
  std::unique_ptr<ExtendedMapping> get_submapping(Span span) const;
  void shift_positions(int shift);

  bool operator==(const ExtendedMapping& other) const;
  friend std::ostream& operator<<(std::ostream& os,
                                  ExtendedMapping& extended_mapping);

 private:
  void process_annotation(const Mapping::Annotation& annotation,
                          std::map<int, std::vector<Span>>& spans_map,
                          std::map<int, Span>& spans_buffer) const;
  inline bool is_open_code(int bitset_index) const;
  void add_span_with_opened_position(int variable_id, int document_position,
                                     std::map<int, Span>& spans_buffer) const;
  void update_last_span_with_closed_position(
      std::map<int, std::vector<Span>>& spans_map, int variable_id,
      int document_position, std::map<int, Span>& spans_buffer) const;

  std::vector<Mapping::Annotation> inverted_annotations_ = {};
};

}  // namespace REmatch

#endif
