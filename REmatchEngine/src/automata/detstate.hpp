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


// class DetState {
//   friend class DFA;
//  public:
//   DetState();
//   DetState(SetState &ss);

//   DetState* next_state(BitsetWrapper char_bitset) const;
//   // Obtain the next state after reading byte c.
//   DetState* next_state(char c) const;

//   // Previous list is used at evaluation context.
//   NodeList* get_previous_list(int i) const;

//   void set_final(bool b);
//   void set_subset(SetState &ss);

//   std::string label() const {return label_;}

//   friend std::ostream& operator<<(std::ostream &os, DetState const &state);

//  private:
//   // Adds a capture transition to a state.
//   bool add_capture(std::bitset<32> S, DetState* state);

//   // Adds a filter transition to a state.
//   bool add_filter(char c, DetState* state);

//   uint_fast32_t id_;

//   std::string label_;

//   std::unordered_map<BitsetWrapper, DetState*> filters_;
//   std::unordered_map<BitsetWrapper, DetState*> one_reached_:

// }; // class DetState

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
    uint64_t visited;  // Mark the reading iteration for which the State is prepared
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
