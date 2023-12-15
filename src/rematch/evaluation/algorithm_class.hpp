#ifndef ALGORITHM_CLASS_HPP
#define ALGORITHM_CLASS_HPP

#include "extended_va/dfa/extended_det_va.hpp"
#include "output_enumeration/ecs.hpp"
#include "output_enumeration/enumerator.hpp"
#include "library_interface/flags.hpp"
#include "tracy/Tracy.hpp"

namespace rematch {
using namespace REMatch;

class AlgorithmClass {
 public:
  AlgorithmClass(ExtendedDetVA& extended_det_va, std::string_view document,
                 Flags flags = Flags());

  void initialize_algorithm();
  void set_ecs(ECS& ecs);
  void set_document(std::string document);
  const Mapping* get_next_mapping();
  size_t get_extended_det_va_size();
  size_t get_extended_va_size();
  size_t get_amount_of_nodes_allocated();
  size_t get_amount_of_nodes_reused();
  size_t get_amount_of_nodes_used();

 private:
  uint64_t pos_i_ = 0;
  std::string document_;

  ExtendedDetVA& extended_det_va_;
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
                   std::vector<CaptureSubsetPair> capture_subset_pairs);
  void update_output_nodes(ExtendedDetVAState*& next_state,
                           ECSNode*& next_node);

  ECSNode* create_root_node_to_enumerate();
  void swap_state_lists();
};

}

#endif
