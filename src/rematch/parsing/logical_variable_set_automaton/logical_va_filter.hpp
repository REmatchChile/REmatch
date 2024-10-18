#ifndef LOGICAL_VA_FILTER
#define LOGICAL_VA_FILTER

#include "parsing/charclass.hpp"

namespace REmatch {
inline namespace parsing {

class LogicalVAState;

class LogicalVAFilter {
public:
  LogicalVAState* from;
  LogicalVAState* next;
  CharClass charclass;

  LogicalVAFilter(
      LogicalVAState* from, CharClass charclass, LogicalVAState* next):
      from(from), next(next), charclass(charclass) {}

  void reset_states(LogicalVAState *s) {next = s;}

  bool operator==(const LogicalVAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (charclass == rhs.charclass);
  }

};

}
}
#endif
