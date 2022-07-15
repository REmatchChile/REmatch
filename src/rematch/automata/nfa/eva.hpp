#ifndef AUTOMATA_NFA_EVA_HPP
#define AUTOMATA_NFA_EVA_HPP

#include <bitset>
#include <list>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>

#include "regex/regex_options.hpp"

namespace rematch {

class State;
class VariableFactory;
class FilterFactory;
class LogicalVA;
class LVACapture;

using CaptureVector = std::vector<std::shared_ptr<LVACapture>>;
using CaptureList = std::list<std::shared_ptr<LVACapture>>;
using CapturePtr = std::shared_ptr<LVACapture>;

// @brief An eVA that builds its graph from a VA.
class ExtendedVA {

public:
  std::vector<State *> states;

  std::unordered_map<unsigned int, State *> idMap;

  ExtendedVA(LogicalVA const &A, Anchor a);
  ExtendedVA();

  // ~ExtendedVA();

  State *init_state() const { return init_state_; }

  void set_initial(State *s) { init_state_ = s; }

  void set_accepting(State *s) { accepting_state_ = s; }

  bool is_static() const { return is_static_; }
  void set_is_static(bool b) { is_static_ = b; }

	std::bitset<32> static_capture() const { return static_S_; }

  std::shared_ptr<VariableFactory> varFactory() const {
    return variable_factory_;
  }
  std::shared_ptr<FilterFactory> filterFactory() const {
    return filter_factory_;
  }

  void addCapture(State *state, std::bitset<32> bs, State *next);

  void trim();

  size_t size() const;

  void captureClosure();

  void cleanUselessCaptureStates();
  void cleanUselessCaptureTransitions();

  std::queue<State *> invTopologicalSort();
  void invTopologicalSortUtil(State *state, std::queue<State *> *Q);

  void relabelStates();

  std::set<State *> get_subset(std::vector<bool> bs) const;

  // @brief Computes the inverse topological sort of the captures.
  // @return A vector of the reverse topological sorting of all capture
  //         transitions
  CaptureVector inverse_topological_sort_captures();

  std::vector<CaptureList> classifySingleCaptures();

  bool offset_possible(CapturePtr capture);
  bool offset_possible(CapturePtr capture1, CapturePtr capture2);
  bool offset_possible(CaptureList &captureList);

  void compute_offset(CaptureList &captureList, int codeIndex);

  void offset_opt();

  void duplicate_opt();

  // Check if the automata is ambiguous.
  bool is_ambiguous() const;

  friend std::ostream &operator<<(std::ostream &os, ExtendedVA const &A);

private:
  void getInvTopSortCapturesUtil(CapturePtr &cap, CaptureVector &L);
  CaptureVector reachableCaptures(CapturePtr &cap);
  bool isReachable(State *from, State *to);

	bool check_if_static();

  State *init_state_;
  State *accepting_state_;

  std::shared_ptr<VariableFactory> variable_factory_;
  std::shared_ptr<FilterFactory> filter_factory_;

  Anchor anchor_;

	bool is_static_{false};
	std::bitset<32> static_S_{0};

  size_t currentID = 0;
};

} // end namespace rematch

#endif // AUTOMATA_NFA_EVA_HPP
