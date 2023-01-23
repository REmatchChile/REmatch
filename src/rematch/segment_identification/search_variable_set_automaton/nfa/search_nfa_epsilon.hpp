#ifndef search_nfa_epsilon
#define search_nfa_epsilon
namespace rematch {

class SearchNFAState;

class SearchNFAEpsilon {
 public:
  SearchNFAState* next;
  SearchNFAState* from;
 private:


 public:
  SearchNFAEpsilon(SearchNFAState* from, SearchNFAState* next): next(next), from(from) {}
  void reset_states(SearchNFAState *s) {next = s;}

 private:
};


}
#endif
