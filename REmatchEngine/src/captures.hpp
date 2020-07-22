#ifndef CAPTURES_HPP
#define CAPTURES_HPP

#include <bitset>
#include "structures.hpp"

// Forward declarations
class DetState;

class Capture {
  public:
    std::bitset<32> S;  // bitset representation of the marker-set
    DetState* next;

    Node* getNewNode(int i, NodeList* copiedList);
    Capture(std::bitset<32> S, DetState* next);
};

#endif