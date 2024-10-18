#ifndef LOGICAL_VA_ANCHOR
#define LOGICAL_VA_ANCHOR

namespace REmatch {
inline namespace parsing {

class LogicalVAState;

class LogicalVAAnchor {
private:
  bool is_start_;

public:
  LogicalVAState* from;
  LogicalVAState* next;

  bool useful = false;

  LogicalVAAnchor(
      LogicalVAState* from, LogicalVAState* next):
      from(from), next(next) {}

  void reset_states(LogicalVAState *s) {next = s;}

  bool operator==(const LogicalVAAnchor &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (is_start_ == rhs.is_start_);
  }

  void set_start(bool start) {is_start_ = start;}
  bool is_start() {return is_start_;}
};

}
}
#endif
