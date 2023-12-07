#ifndef CAPTURE_SUBSET_PAIR_HPP
#define CAPTURE_SUBSET_PAIR_HPP

#include <bitset>

namespace rematch {

class ExtendedDetVAState;

struct CaptureSubsetPair {
  std::bitset<64> capture;
  ExtendedDetVAState* subset;
};
}  // namespace rematch

#endif
