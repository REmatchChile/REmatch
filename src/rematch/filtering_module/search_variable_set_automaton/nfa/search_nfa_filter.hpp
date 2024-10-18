#ifndef search_nfa_filter
#define search_nfa_filter

#include "parsing/charclass.hpp"

namespace REmatch {
inline namespace filtering_module {

class SearchNFAState;

class SearchNFAFilter {
 public:
  SearchNFAState* from;
  SearchNFAState* next;
  CharClass charclass;
  bool flag;

 private:

 public:
  SearchNFAFilter(SearchNFAState* from, CharClass charclass,
                  SearchNFAState* next) :
    from(from), next(next), charclass(charclass) {}

  void reset_states(SearchNFAState *s) {next = s;}

  bool operator==(const SearchNFAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (charclass == rhs.charclass);
  }

 private:

};
}
}
#endif
