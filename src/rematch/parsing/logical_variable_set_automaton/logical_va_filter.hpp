#ifndef LOGICAL_VA_FILTER
#define LOGICAL_VA_FILTER

namespace rematch {

class LogicalVAState;

class LogicalVAFilter {
public:
  LogicalVAState* from;
  LogicalVAState* next;
  unsigned int code;
  bool flag;

  LogicalVAFilter(
      LogicalVAState* from, unsigned int coding, LogicalVAState* next):
      from(from), next(next), code(coding) {}

  void reset_states(LogicalVAState *s) {next = s;}

  bool operator==(const LogicalVAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (code == rhs.code);
  }

};

}
#endif
