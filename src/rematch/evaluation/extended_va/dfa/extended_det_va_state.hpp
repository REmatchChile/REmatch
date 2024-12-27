#ifndef EXTENDED_DET_VA_STATE_HPP
#define EXTENDED_DET_VA_STATE_HPP

#include <optional>
#include <set>

#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "output_enumeration/ecs.hpp"
#include "aliases.hpp"
#include "capture_subset_pair.hpp"

namespace REmatch {

class ExtendedDetVAState {
 private:
  static unsigned int ID;
  bool is_initial_ = false;
  bool is_accepting_ = false;

  std::vector<ExtendedVAState*> states_subset_;

 public:
  uint32_t id;
  std::vector<std::optional<std::vector<CaptureSubsetPair>>> cached_transitions{256, std::nullopt};
  ECSNode* output_node = nullptr;
  int64_t phase = -1;

  ExtendedDetVAState();

  explicit ExtendedDetVAState(StatesPtrSet &states_subset);

  std::optional<std::vector<CaptureSubsetPair>> get_transition(char letter);
  void cache_transition(
      char letter,
      std::optional<std::vector<CaptureSubsetPair>> capture_subset_pairs);

  bool is_initial() const;
  void set_initial(bool initial);
  bool is_accepting() const;
  void set_node(ECSNode* node);
  void unset_node();
  ECSNode* get_node() const;
  void set_phase(int64_t phase);

  inline size_t get_subset_size() {return states_subset_.size(); }

  std::vector<ExtendedVAState*> get_states_subset() const { return states_subset_; }
};

}  // namespace REmatch

#endif
