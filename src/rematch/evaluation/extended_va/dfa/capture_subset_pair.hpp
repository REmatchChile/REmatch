
#include <bitset>

namespace rematch {

class ExtendedDetVAState;

struct CaptureSubsetPair {
  std::bitset<64> capture;
  ExtendedDetVAState* subset;
};
}  // namespace rematch
