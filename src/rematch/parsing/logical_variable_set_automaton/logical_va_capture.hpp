#ifndef LOGICAL_VA_CAPTURE_HPP
#define LOGICAL_VA_CAPTURE_HPP
#include <bitset>
namespace REmatch {
inline namespace parsing {
  class LogicalVAState;

  class LogicalVACapture{
  public:
    LogicalVAState* from;
    LogicalVAState* next;
    std::bitset<64> code;
    bool flag;

    LogicalVACapture(LogicalVAState* from, std::bitset<64> coding, LogicalVAState* next);

    void reset_states(LogicalVAState* insiding, LogicalVAState *to) {from = insiding; next=to;}

    bool operator==(const LogicalVACapture &rhs) const;

    bool operator<(const LogicalVACapture &rhs) const;
  };
}
}
#endif
