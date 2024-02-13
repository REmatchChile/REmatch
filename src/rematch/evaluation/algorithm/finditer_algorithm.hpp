#ifndef FINDITER_ALGORITHM_HPP
#define FINDITER_ALGORITHM_HPP

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va_state.hpp"

namespace rematch {

class FinditerAlgorithm : public AlgorithmClass {
 public:
  FinditerAlgorithm(ExtendedVA& extended_va, std::shared_ptr<Document> document,
                    Flags flags = Flags());

  const Mapping* get_next_mapping() override;

 private:
  void evaluate();
  void update_output_nodes(ExtendedDetVAState*& next_state,
                           ECSNode*& next_node) override;
  void enumerate();
  ECSNode* create_root_node_to_enumerate();
};

}  // namespace rematch

#endif
