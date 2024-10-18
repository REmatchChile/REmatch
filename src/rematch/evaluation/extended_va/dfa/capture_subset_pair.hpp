#ifndef CAPTURE_SUBSET_PAIR_HPP
#define CAPTURE_SUBSET_PAIR_HPP

#include <bitset>

namespace REmatch {

class ExtendedDetVAState;

struct CaptureSubsetPair {
  std::bitset<64> capture;
  ExtendedDetVAState* subset;
};
}  // namespace REmatch

#endif
