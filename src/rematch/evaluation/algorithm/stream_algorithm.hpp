#include "REmatch/flags.hpp"
#include "algorithm_class.hpp"

#include "evaluation/document.hpp"
#include "evaluation/stream.hpp"
#include "library_interface/reader.hpp"

using namespace rematch;

class StreamAlgorithm {
 public:
  StreamAlgorithm(ExtendedVA& extended_va, std::shared_ptr<Stream> stream,
                  Flags flags = Flags())
      : stream_(std::move(stream)), extended_det_va_(extended_va, flags) {
    ECS_interface_ = std::make_unique<ECS>(flags);
    enumerator_ = std::make_unique<Enumerator>();

    ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
    ECSNode* bottom_node = ECS_interface_->create_bottom_node();
    initial_state->set_node(bottom_node);
    ECS_interface_->pin_node(bottom_node);

    initialize_algorithm();
  }

  void initialize_algorithm() {
    extended_det_va_.set_state_initial_phases();
    ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
    current_states_.push_back(initial_state);
  }

  const Mapping* get_next_mapping() {
    if (enumerator_->has_next()) {
      return enumerator_->next();
    }
    enumerator_->reset();

    evaluate();
    enumerate();

    if (enumerator_->has_next())
      return enumerator_->next();

    return nullptr;
  }

  void evaluate() {
    while (stream_->read(current_char)) {
      evaluate_single_character();
      swap_state_lists();
      pos_i_++;

      if (!reached_final_states_.empty())
        return;
    }
  }

  void evaluate_single_character() {
    for (auto& current_state : current_states_) {

      std::vector<CaptureSubsetPair> capture_subset_pairs =
          extended_det_va_.get_next_states(current_state, current_char);

      if (!capture_subset_pairs.empty()) {
        update_sets(current_state, capture_subset_pairs);
      } else {
        ECS_interface_->unpin_node(current_state->output_node);
      }
    }
  }

  void update_sets(ExtendedDetVAState*& current_state,
                   std::vector<CaptureSubsetPair> capture_subset_pairs) {

    auto it = capture_subset_pairs.begin();

    // handle the empty capture
    if (capture_subset_pairs[0].capture.none()) {
      ExtendedDetVAState* next_state = capture_subset_pairs[0].subset;

      ECSNode* next_node = current_state->get_node();
      update_output_nodes(next_state, next_node);

      it++;
    }

    // handle not empty captures, skip first pair if already updated
    for (; it != capture_subset_pairs.end(); it++) {
      auto pair = *it;
      ExtendedDetVAState* next_state = pair.subset;
      std::bitset<64> capture = pair.capture;

      ECSNode* next_node = ECS_interface_->create_extend_node(
          current_state->get_node(), capture, pos_i_);
      update_output_nodes(next_state, next_node);
    }

    ECS_interface_->unpin_node(current_state->get_node());
  }

  void update_output_nodes(ExtendedDetVAState*& next_state,
                           ECSNode*& next_node) {
    if (next_state->phase < (int64_t)pos_i_) {
      next_state->set_phase(pos_i_);

      next_state->set_node(next_node);
      ECS_interface_->pin_node(next_node);

      if (next_state->is_accepting()) {
        reached_final_states_.push_back(next_state);
      } else {
        next_states_.push_back(next_state);
      }

    } else {
      ECSNode* current_next_node = next_state->get_node();
      ECSNode* union_node =
          ECS_interface_->create_union_node(current_next_node, next_node);

      ECS_interface_->unpin_node(current_next_node);
      next_state->set_node(union_node);
      ECS_interface_->pin_node(union_node);
    }
  }

  void enumerate() {
    create_root_node_to_enumerate();

    if (ECS_root_node_ != nullptr)
      enumerator_->add_node(ECS_root_node_);
  }

  ECSNode* create_root_node_to_enumerate() {
    if (ECS_root_node_ != nullptr) {
      ECS_interface_->unpin_node(ECS_root_node_);
      ECS_root_node_ = nullptr;
    }

    for (auto& state : reached_final_states_) {
      if (ECS_root_node_ == nullptr) {
        // no need to pin/unpin the node here, it reuses the reference of the final state
        ECS_root_node_ = state->get_node();
      } else {
        ECSNode* union_node = ECS_interface_->create_union_node(
            ECS_root_node_, state->get_node());
        ECS_interface_->pin_node(union_node);
        ECS_interface_->unpin_node(ECS_root_node_);
        ECS_interface_->unpin_node(state->get_node());
        ECS_root_node_ = union_node;
      }
    }
    reached_final_states_.clear();
    return ECS_root_node_;
  }

  void swap_state_lists() {
    current_states_.swap(next_states_);
    next_states_.clear();
  }

  char current_char{};

  uint64_t pos_i_ = 0;
  uint64_t doc_start_i_ = 0;
  uint64_t doc_end_i_ = 0;

  std::shared_ptr<Stream> stream_;

  ExtendedDetVA extended_det_va_;
  std::unique_ptr<ECS> ECS_interface_;
  std::unique_ptr<Enumerator> enumerator_;
  ECSNode* ECS_root_node_ = nullptr;

  std::vector<ExtendedDetVAState*> current_states_ = {};
  std::vector<ExtendedDetVAState*> next_states_ = {};
  std::vector<ExtendedDetVAState*> reached_final_states_ = {};
};
