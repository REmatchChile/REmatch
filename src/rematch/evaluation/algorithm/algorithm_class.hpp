#pragma once

#include <memory>

#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/enumerator.hpp"

namespace REmatch {
class Document;

using namespace REmatch;

class AlgorithmClass {
 public:
  AlgorithmClass(ExtendedVA& extended_va, std::shared_ptr<Document> document,
                 uint_fast32_t max_mempool_duplications, uint_fast32_t max_deterministic_states);

  void initialize_algorithm();
  ECS& get_ecs();
  void set_document_indexes(Span& span);
  void set_null_segment();
  virtual const Mapping* get_next_mapping() = 0;

  size_t get_extended_det_va_size();
  size_t get_extended_va_size();
  size_t get_amount_of_nodes_allocated();
  size_t get_amount_of_nodes_reused();
  size_t get_amount_of_nodes_used();

  virtual ~AlgorithmClass() = default;

 protected:
  uint64_t pos_i_ = 0;
  uint64_t doc_start_i_ = 0;
  uint64_t doc_end_i_ = 0;

  std::shared_ptr<Document> document_;

  ExtendedDetVA extended_det_va_;
  std::unique_ptr<ECS> ECS_interface_;
  std::unique_ptr<Enumerator> enumerator_;
  ECSNode* ECS_root_node_ = nullptr;

  std::vector<ExtendedDetVAState*> current_states_ = {};
  std::vector<ExtendedDetVAState*> next_states_ = {};
  std::vector<ExtendedDetVAState*> reached_final_states_ = {};

  void evaluate_single_character();
  void update_sets(ExtendedDetVAState*& current_state,
                   std::vector<CaptureSubsetPair> capture_subset_pairs);
  virtual void update_output_nodes(ExtendedDetVAState*& next_state,
                                   ECSNode*& next_node) = 0;

  void swap_state_lists();
};

}  // namespace REmatch
