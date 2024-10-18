#ifndef SRC_REMATCH_AUTOMATA_NFA_STATE_HPP
#define SRC_REMATCH_AUTOMATA_NFA_STATE_HPP

#include <list>
#include <set>
#include <bitset>
#include <memory>
#include "parsing/charclass.hpp"
#include "parsing/logical_variable_set_automaton/logical_va_state.hpp"
#include "filtering_module/search_variable_set_automaton/nfa/search_nfa_filter.hpp"

namespace REmatch {
inline namespace filtering_module {

class SearchNFAState;

enum SearchNFAStateFlags {
  kDefaultSearchNFAState = 0,
  kFinalSearchNFAState = 1,
  kSuperFinalSearchNFAState = 1 << 1,
  kInitialSearchNFAState = 1 << 2,
};

class SearchNFAState {
  public:
    // Booleans for graph algorithms
    bool tempMark = false;
    char colorMark = 'w';
    uint32_t visitedBy = 0;
    uint32_t flags;
    unsigned int id;

    std::list<SearchNFAFilter*> filters;    // Filter array
    std::list<SearchNFAFilter*> backward_filters_;

  private:
    static unsigned int ID; // Static var to make sequential id's

  public:
    SearchNFAState();

    ~SearchNFAState();

    SearchNFAState(const SearchNFAState& s);

    SearchNFAState(const LogicalVAState* state) :
      flags(state->flags), id(ID++)
    {
      set_initial(state->initial());
      set_accepting(state->accepting());
    }

    void init();

    SearchNFAState* nextFilter(CharClass charclass);

    void add_filter(CharClass charclass, SearchNFAState* next);

    bool initial() const { return flags & kInitialSearchNFAState; }
    void set_initial(bool b);

    bool accepting() const { return flags & kFinalSearchNFAState; }
    void set_accepting(bool b);

    bool super_final() const { return flags & kSuperFinalSearchNFAState; }

    bool operator==(const SearchNFAState &rhs) const;
};

}
}

#endif
