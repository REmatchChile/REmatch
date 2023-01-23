#include "segment_identification/search_variable_set_automaton/nfa/search_nfa_capture.hpp"

namespace rematch {

SearchNFACapture::SearchNFACapture(SearchNFAState* from, std::bitset<64> coding, SearchNFAState* next):
    from(from), next(next), code(coding) {}

bool SearchNFACapture :: operator==(const SearchNFACapture &rhs) const {
  return (code == rhs.code && next == rhs.next && from == rhs.from);
}

bool SearchNFACapture :: operator<(const SearchNFACapture &rhs) const {
  /* Simple ordering for being able to construct an std::set<SearchNFACapture> */
  if(code.to_ulong() != rhs.code.to_ulong()) {
    return code.to_ulong() < rhs.code.to_ulong();
  }

  else if(next != rhs.next) {
    return next < rhs.next;
  }

  return from < rhs.from;
}

}
