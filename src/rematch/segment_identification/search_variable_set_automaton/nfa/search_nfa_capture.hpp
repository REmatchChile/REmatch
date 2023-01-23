#ifndef search_nfa_capture
#define search_nfa_capture
#include "bitset"

class SearchNFAState;

namespace rematch {

class SearchNFACapture{
 public:
  SearchNFAState* from;
  SearchNFAState* next;
  std::bitset<64> code;
  bool flag;

 private:

 public:
  SearchNFACapture(SearchNFAState* from, std::bitset<64> coding, SearchNFAState* next);
  void reset_states(SearchNFAState* insiding, SearchNFAState *to) {from = insiding; next=to;}
  bool operator==(const SearchNFACapture &rhs) const;
  bool operator<(const SearchNFACapture &rhs) const;

 private:
};

}
#endif
