#ifndef AUTOMATA_DFA_DSTATE_HPP
#define AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <set>

#include "output_enumeration/ecs.hpp"
#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_transition.hpp"
#include "segment_identification/search_variable_set_automaton/nfa/search_nfa_state.hpp"

namespace rematch {

class SearchDFAState {
 public:

  ECSNode* node;
  int visited = -1;

  enum Flags {
    kDefaultSearchNFAState    =  0,
    kAcceptingSearchNFAState  =  1,
    kInitialSearchNFAState    =  kAcceptingSearchNFAState << 1,
    kLeftAntiAnchor  =  kInitialSearchNFAState  << 1
  };

  SearchDFAState(size_t tot_states);
  SearchDFAState(size_t tot_states, std::vector<SearchNFAState*> states);
  SearchDFAState(size_t tot_states, std::set<SearchNFAState*> states);

  void add_state(SearchNFAState* p);

  void pass_node(ECSNode* n);

  std::vector<bool> bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<SearchNFAState*> Subset of NFA states
  std::vector<SearchNFAState*> subset() const { return states_subset_; }

  // @brief Returns the subset representation string (i.e. {q1,q2,...}).
  // @return std::string Subset representation of the SearchDFAState.
  std::string label() const { return label_; }

  int id() const { return id_; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const { return  flags & kAcceptingSearchNFAState; }
  void set_accepting(bool b);

  bool initial() const { return flags & kInitialSearchNFAState; }
  void set_initial(bool b);

  void add_direct(char a, SearchDFAState* q);
  void add_capture(char a, std::bitset<64> S, SearchDFAState* q);
  void add_empty(char a);

  Transition* next_transition(char a) const {
    return transitions_[a].get();
  }

 private:
  static int ID;

  uint id_;

  std::vector<std::unique_ptr<Transition>> transitions_;

  std::string label_;

  std::vector<bool> states_bitmap_;

  std::vector<SearchNFAState*> states_subset_;

  uint8_t flags = kDefaultSearchNFAState;

}; // end class SearchDFAState

} // end namespace rematch


#endif // AUTOMATA_DFA_DSTATE_HPP
