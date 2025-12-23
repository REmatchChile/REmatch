#pragma once

#include <memory>
#include <vector>

#include "evaluation/document.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/enumerator.hpp"

namespace REmatch {

class AlgorithmClass {
 public:
  virtual void begin(std::shared_ptr<TextWrapper> document) = 0;
  virtual const Mapping* next() = 0;

  ECS& get_ecs() const;
  size_t get_extended_det_va_size() const;
  size_t get_extended_va_size() const;
  size_t get_amount_of_nodes_allocated() const;
  size_t get_amount_of_nodes_reused() const;
  size_t get_amount_of_nodes_used() const;

  virtual ~AlgorithmClass() = default;

 protected:
  std::unique_ptr<ExtendedDetVA> extended_det_va_;
  std::unique_ptr<ECS> ECS_interface_;
  std::unique_ptr<Enumerator> enumerator_;

  std::vector<ExtendedDetVAState*> current_states_ = {};
  std::vector<ExtendedDetVAState*> next_states_ = {};
  std::vector<ExtendedDetVAState*> reached_final_states_ = {};

  void swap_state_lists();
};

}  // namespace REmatch
