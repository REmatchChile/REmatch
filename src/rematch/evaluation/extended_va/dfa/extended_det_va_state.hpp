#ifndef EXTENDED_DET_VA_STATE_HPP
#define EXTENDED_DET_VA_STATE_HPP

#include <set>
#include "evaluation/extended_va/nfa/extended_va.hpp"

namespace rematch {

struct CaptureSubsetPair;

class ExtendedDetVAState {
 private:
  static unsigned int ID;
  bool is_initial_ = false;

  std::vector<ExtendedVAState*> states_subset_;

 public:
  uint id;
  std::map<char, std::vector<CaptureSubsetPair*>> cached_transitions;

  ExtendedDetVAState();
  ExtendedDetVAState(std::set<ExtendedVAState*> &states_subset);

  void add_to_subset(ExtendedVAState* state);
  bool contains_cached_transition(char letter);
  std::vector<CaptureSubsetPair*> get_transition(char letter);

  bool is_initial();
  void set_initial(bool initial);
  int get_subset_size() {return states_subset_.size(); }

  std::vector<ExtendedVAState*> get_states_subset() const { return states_subset_; }
};

}  // namespace rematch

#endif
