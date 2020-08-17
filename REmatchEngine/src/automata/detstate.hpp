#ifndef DETSTATE_HPP
#define DETSTATE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <bitset>

#include "bitsetwrapper.hpp"
#include "structures.hpp"
#include "det/setstate.hpp"
#include "captures.hpp"

namespace rematch {
  class Transition;
}

using namespace rematch;

class DetState {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    /* Unique ID*/
    unsigned int id;                              // id

    // Transitions to other states
    std::array<Transition*, 128> transitions_;

    // Label used for debugging
    std::string label;

    int64_t visited;  // Mark the reading iteration for which the State is prepared
    NodeList* currentL;
    NodeList* copiedList;

    SetState* ss;

    bool isFinal, isSuperFinal, mark;

    DetState();
    DetState(SetState* ss);

    Transition* next_transition(char a);

    void add_capture(char a, std::bitset<32> S, DetState* state);
    void add_direct(char a, DetState* state);

    DetState* nextState(BitsetWrapper charBitset);
    DetState* nextState(char a);
    NodeList* getPreviousList(int i);
    void setFinal(bool b);
    void setSubset(SetState* newss);
    std::string pprint();
    friend std::ostream & operator<<(std::ostream &os, DetState const &bs);
};


std::ostream & operator<<(std::ostream &os, DetState const &q);

#endif
