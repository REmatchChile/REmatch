#ifndef EXTENDED_VA_CAPTURE_HPP
#define EXTENDED_VA_CAPTURE_HPP
#include <bitset>

namespace rematch {

  class ExtendedVAState;

  class ExtendedVACapture{
  public:
    ExtendedVAState* from;
    ExtendedVAState* next;
    std::bitset<64> code;

    ExtendedVACapture(ExtendedVAState* from, std::bitset<64> coding, ExtendedVAState* next);

    bool operator==(const ExtendedVAState &rhs) const;

    bool operator<(const ExtendedVAState &rhs) const;
  };
}

#endif
