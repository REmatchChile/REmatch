#pragma once

#include <cstdint>

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/enumerator.hpp"

namespace REmatch {

class FindoneAlgorithm : public AlgorithmClass {
 public:
  FindoneAlgorithm(ExtendedVA& extended_va, std::shared_ptr<Document> document,
                   uint_fast32_t max_mempool_duplications,
                   uint_fast32_t max_deterministic_states);

  const Mapping* get_next_mapping() override;

 private:
  void evaluate();
  void update_output_nodes(ExtendedDetVAState*& next_state,
                           ECSNode*& next_node) override;
  void enumerate();
};

}  // namespace REmatch
