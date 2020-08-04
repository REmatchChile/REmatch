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

class DetState {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    /* Unique ID*/
    unsigned int id;                              // id

    // Transitions to other states
    // TODO: Maybe it's better performance to have a union of 1-capture vs multi-capture
    std::array<std::vector<Capture*>, 128> transitions_;

    // Indices of currently held capture-transitions. Used for faster iteration.
    // TODO: This is memory intensive. Imagine a state with a dot transition,
    //       then transition-wise it'll allocate 128*(1+4*Captures) bytes.
    // TODO: Measure memory used by the DFA.
    // TODO: Maybe think about using filter-wise indexing instead of byte-wise.
    std::vector<uint8_t> capture_indices_;

    // Label used for debugging
    std::string label;

    int64_t visited;  // Mark the reading iteration for which the State is prepared
    NodeList* currentL;
    NodeList* copiedList;

    SetState* ss;

    bool isFinal, isSuperFinal, mark;

    DetState();
    DetState(SetState* ss);

    std::vector<Capture*>& next_captures(char a);

    void add_transition(char a, std::bitset<32> S, DetState* state);

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
