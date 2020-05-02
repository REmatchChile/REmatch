#ifndef DETSTATE_HPP
#define DETSTATE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <bitset>

#include "bitsetwrapper.hpp"

class Capture;
class NodeList;
class StateList;
class SetState;
class Node;

class DetState {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    /* Unique ID*/
    unsigned int id;                              // id
    std::string label;

    /* Transitions */
    std::vector<Capture*> c;                    // Capture pointers array

    std::unordered_map<BitsetWrapper, DetState*> filters;

    std::unordered_map<BitsetWrapper, DetState*> oneReached;


    /* Evaluation algorithm vars */
    long visited;  // Mark the reading iteration for which the State is prepared
    NodeList* currentL;
    NodeList* copiedList;

    #ifdef NOPT_CROSSPROD
    NodeList* oldL;
    #endif

    /* Determination variables*/
    SetState* ss;


    bool isFinal, isSuperFinal, mark;

    DetState  *singleFilters[128];

    DetState();
    DetState(SetState* ss);

    DetState* nextState(BitsetWrapper charBitset);
    DetState* nextState(char a);
    NodeList* getPreviousList(int i);
    Node* getPreviousHead(int i);
    Node* getPreviousTail(int i);
    void addCapture(std::bitset<32> S, DetState* next);
    void addFilter(BitsetWrapper charBitset, DetState* nextState);
    void addFilter(char a, DetState *nextState);
    void setFinal(bool b);
    void setSubset(SetState* newss);
    std::string pprint();
    friend std::ostream & operator<<(std::ostream &os, DetState const &bs);
};


std::ostream & operator<<(std::ostream &os, DetState const &q);

#endif
