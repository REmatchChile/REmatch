#pragma once

#include <cstdint>

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/enumerator.hpp"

#include <REmatch/constants.hpp>
namespace REmatch {

class FindoneAlgorithm : public AlgorithmClass {
 public:
  explicit FindoneAlgorithm(
      ExtendedVA& extended_va,
      uint_fast32_t max_mempool_duplications = REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
      uint_fast32_t max_deterministic_states = REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  void begin(std::shared_ptr<TextWrapper> document) override;
  const Mapping* next() override;
  void evaluate_start_char();
  void set_document_indexes(Span& span);

 private:
  void evaluate();
  void evaluate_single_character();
  void update_sets(ExtendedDetVAState* current_state,
                   const std::vector<CaptureSubsetPair>& capture_subset_pairs);
  void update_output_nodes(ExtendedDetVAState* next_state, ECSNode* next_node);
  void enumerate();

  std::shared_ptr<TextWrapper> document_;
  char current_char = 0;
  uint64_t pos_i_ = 0;
  uint64_t doc_start_i_ = 0;
  uint64_t doc_end_i_ = 0;
};

}  // namespace REmatch
