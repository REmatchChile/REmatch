#ifndef ALGORITHM_CLASS_HPP
#define ALGORITHM_CLASS_HPP

#include "extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/enumerator.hpp"

namespace rematch {

class AlgorithmClass {
 public:
  AlgorithmClass(ExtendedDetVA &extended_det_va, std::string_view document);

  void initialize_algorithm();
  void set_ecs(ECS& ecs);
  const Mapping* get_next_mapping();
  void clear_state_node_links();

 private:
  uint64_t pos_i_ = 0;
  std::string_view document_;

  ExtendedDetVA extended_det_va_;
  ECS* ECS_interface_;
  Enumerator* enumerator_;
  ECSNode* ECS_root_node_ = nullptr;

  std::vector<ExtendedDetVAState*> current_states_ = {};
  std::vector<ExtendedDetVAState*> next_states_ = {};
  std::vector<ExtendedDetVAState*> reached_final_states_ = {};

  void evaluate();
  void evaluate_single_character();
  void enumerate();
  void update_sets(ExtendedDetVAState*& current_state,
                   std::vector<CaptureSubsetPair*> capture_subset_pairs);
  void update_output_nodes(ExtendedDetVAState*& next_state,
                           ECSNode*& next_node);

  ECSNode* create_root_node_to_enumerate();
  void swap_state_lists();
  void link_node_to_state(ExtendedDetVAState* state, ECSNode* node);
};

}

#endif
