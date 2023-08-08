#ifndef EXTENDED_VA_FILTER
#define EXTENDED_VA_FILTER

#include "../parsing/charclass.hpp"
#include "extended_va_state.hpp"

namespace rematch {

class ExtendedVAFilter {
public:
  ExtendedVAState* from;
  ExtendedVAState* next;
  CharClass charclass;

  ExtendedVAFilter(
      ExtendedVAState* from, CharClass charclass, ExtendedVAState* next):
      from(from), next(next), charclass(charclass) {}

  bool operator==(const ExtendedVAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (charclass == rhs.charclass);
  }

};
}

#endif
