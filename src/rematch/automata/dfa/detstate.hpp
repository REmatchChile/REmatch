#ifndef SRC_REMATCH_AUTOMATA_DFA_DETSTATE_HPP
#define SRC_REMATCH_AUTOMATA_DFA_DETSTATE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <bitset>
#include <memory>

#include "structs/ecs/ecs.hpp"

namespace rematch {

class Transition;
class BitsetWrapper;
class SetState;

namespace internal {
class FastNodeList;
class NodeList;
}

class DetState {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    /* Unique ID*/
    unsigned int id;                              // id

    // Transitions to other states
    std::array<std::unique_ptr<Transition>, 128> transitions_;

    // Label used for debugging
    std::string label;

    DetState* drop_super_finals_ = nullptr;

    int64_t visited;  // Mark the reading iteration for which the State is prepared

    internal::FastNodeList* currentL;
    internal::FastNodeList* copiedList;

    internal::ECS::Node* currentNode;

    SetState* ss;

    bool isFinal, isSuperFinal, mark, isOnlyInit, hasCapture;

    DetState();
    DetState(SetState* ss);

    Transition* next_transition(char a);

    DetState* drop_super_finals() { return drop_super_finals_;}
    void set_drop_super_finals(DetState* s) {drop_super_finals_ = s;}

    void add_capture(char a, std::bitset<32> S, DetState* state);
    void add_direct(char a, DetState* state);
    void add_empty(char a, DetState* state);

    // Remove SuperFinals states from the detState;
    bool remove_superfinals();

    bool empty() const;

    // Passes an ECS::Node to the state, increasing the Node's reference counter
    void pass_node(internal::ECS::Node* node);

    DetState* nextState(BitsetWrapper charBitset);
    DetState* nextState(char a);
    internal::NodeList* getPreviousList(int i);
    void setFinal(bool b);
    void setSubset(SetState* newss);
    std::string pprint();
    friend std::ostream & operator<<(std::ostream &os, DetState const &bs);
};


std::ostream & operator<<(std::ostream &os, DetState const &q);

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_DFA_DETSTATE_HPP
