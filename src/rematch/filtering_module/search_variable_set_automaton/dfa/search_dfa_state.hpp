#ifndef AUTOMATA_DFA_DSTATE_HPP
#define AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <set>

#include "output_enumeration/ecs.hpp"
#include "filtering_module/search_variable_set_automaton/nfa/search_nfa_state.hpp"

namespace REmatch {
inline namespace filtering_module {

class SearchDFAState {
 public:
  std::vector<SearchDFAState*> transitions;

 private:
  uint32_t id;
  static int ID;
  std::string label_;
  std::vector<SearchNFAState*> states_subset_;
  uint8_t flags = kDefaultSearchNFAState;

 public:

  enum Flags {
    kDefaultSearchNFAState    =  0,
    kAcceptingSearchNFAState  =  1,
    kInitialSearchNFAState    =  1 << 1,
    kEndsNFAState = 1 << 2,
  };

  SearchDFAState();
  SearchDFAState(std::set<SearchNFAState*> &states, bool is_ends);

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<SearchNFAState*> Subset of NFA states
  std::vector<SearchNFAState*> subset() const { return states_subset_; }

  // @brief Returns the subset representation string (i.e. {q1,q2,...}).
  // @return std::string Subset representation of the SearchDFAState.
  std::string label() const { return label_; }

  int get_id() const { return id; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const { return  flags & kAcceptingSearchNFAState; }
  void set_accepting();

  bool initial() const { return flags & kInitialSearchNFAState; }
  void set_initial();

  bool ends() const { return flags & kEndsNFAState; }
  void set_ends();

  void add_direct(char a, SearchDFAState* q);
  void add_capture(char a, std::bitset<64> S, SearchDFAState* q);
  void add_empty(char a);

}; // end class SearchDFAState

}
}


#endif // AUTOMATA_DFA_DSTATE_HPP
