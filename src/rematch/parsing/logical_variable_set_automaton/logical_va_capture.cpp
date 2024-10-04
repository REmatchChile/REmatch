#include "parsing/logical_variable_set_automaton/logical_va_capture.hpp"

namespace REmatch {
inline namespace parsing {

LogicalVACapture::LogicalVACapture(LogicalVAState* from, std::bitset<64> coding, LogicalVAState* next):
    from(from), next(next), code(coding) {}

bool LogicalVACapture :: operator==(const LogicalVACapture &rhs) const {
  return (code == rhs.code && next == rhs.next && from == rhs.from);
}

bool LogicalVACapture :: operator<(const LogicalVACapture &rhs) const {
  /*! Simple ordering for being able to construct an std::set<LogicalVACapture> */
  if(code.to_ulong() != rhs.code.to_ulong()) {
    return code.to_ulong() < rhs.code.to_ulong();
  }

  else if(next != rhs.next) {
    return next < rhs.next;
  }

  return from < rhs.from;
}

}
}
