#ifndef EXTENDED_VA_FILTER_HPP
#define EXTENDED_VA_FILTER_HPP

#include "parsing/charclass.hpp"

namespace REmatch {

class ExtendedVAState;

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
