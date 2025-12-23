#pragma once

#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "evaluation/stream.hpp"
#include "filtering_module/segment_identificator_stream.hpp"
#include "output_enumeration/enumerator.hpp"
#include "output_enumeration/mapping.hpp"

namespace REmatch {

class StreamAlgorithm {
 public:
  StreamAlgorithm(ExtendedVA& extended_va, std::shared_ptr<Stream> stream,
                  uint_fast32_t max_mempool_duplications, uint_fast32_t max_deterministic_states);

  void initialize_algorithm();
  void set_document_indexes(Span& span, SegmentResult segment_result);
  void set_null_segment();
  const Mapping* get_next_mapping();

  void evaluate_start_char();

 private:
  void evaluate();

  void evaluate_single_character();

  void update_sets(ExtendedDetVAState*& current_state,
                   std::vector<CaptureSubsetPair> capture_subset_pairs);

  void update_output_nodes(ExtendedDetVAState*& next_state, ECSNode*& next_node);

  void enumerate();

  ECSNode* create_root_node_to_enumerate();

  void swap_state_lists();

  char current_char{};

  uint64_t pos_i_ = 0;
  uint64_t end_i_ = 0;
  SegmentResult segment_result = DoesNotFitInBuffer;

  std::shared_ptr<Stream> stream_;

  ExtendedDetVA extended_det_va_;
  std::unique_ptr<ECS> ECS_interface_;
  std::unique_ptr<Enumerator> enumerator_;
  ECSNode* ECS_root_node_ = nullptr;

  std::vector<ExtendedDetVAState*> current_states_ = {};
  std::vector<ExtendedDetVAState*> next_states_ = {};
  std::vector<ExtendedDetVAState*> reached_final_states_ = {};
};
}  // namespace REmatch
