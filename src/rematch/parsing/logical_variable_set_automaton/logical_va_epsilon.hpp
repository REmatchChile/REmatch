#ifndef LOGICAL_VA_EPSILON
#define LOGICAL_VA_EPSILON
namespace REmatch {
inline namespace parsing {

class LogicalVAState;

class LogicalVAEpsilon {
public:
  LogicalVAState* next;
  LogicalVAState* from;
  LogicalVAEpsilon(LogicalVAState* from, LogicalVAState* next):
    next(next), from(from) {}

  void reset_states(LogicalVAState *s) {next = s;}
};

}
}
#endif
