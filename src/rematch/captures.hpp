#ifndef CAPTURES_HPP
#define CAPTURES_HPP

#include <bitset>

#include "structs/dag/nodelist.hpp"

namespace rematch {

// Forward declarations
class DetState;

class Capture {
  public:
    std::bitset<32> S;  // bitset representation of the marker-set
    DetState* next;

    internal::Node* getNewNode(int i, internal::NodeList* copiedList);
    Capture(std::bitset<32> S, DetState* next);
}; // end class Capture

} // end namespace rematch

#endif