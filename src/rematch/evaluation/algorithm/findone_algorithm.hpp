#ifndef FINDONE_ALGORITHM_HPP
#define FINDONE_ALGORITHM_HPP

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/enumerator.hpp"

namespace rematch {

class FindoneAlgorithm : public AlgorithmClass {
 public:
  FindoneAlgorithm(ExtendedVA& extended_va, std::shared_ptr<Document> document,
                   Flags flags = Flags());

  const Mapping* get_next_mapping() override;

 private:
  void evaluate();
  void update_output_nodes(ExtendedDetVAState*& next_state,
                           ECSNode*& next_node) override;
  void enumerate();
};

}  // namespace rematch

#endif
