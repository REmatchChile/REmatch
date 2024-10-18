#ifndef EXTENDED_VA_CAPTURE_HPP
#define EXTENDED_VA_CAPTURE_HPP
#include <bitset>

namespace REmatch {

class ExtendedVAState;

class ExtendedVACapture {
 public:
  ExtendedVAState* from;
  std::bitset<64> code;
  ExtendedVAState* next;

  ExtendedVACapture(ExtendedVAState* from, std::bitset<64> code,
                    ExtendedVAState* next)
      : from(from), code(code), next(next) {}
};
}  // namespace REmatch

#endif
