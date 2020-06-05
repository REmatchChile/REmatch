// structures.hpp: Javier G. Montoya S. ; Nicolás A. Van Sint Jan C.
// Description: Implements the structures needed for the SpannersConst algo.

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <bitset>

#include "factories/factories.hpp"

class NodeList;

class Node {
public:
  std::bitset<32> S;
  /* S represents a subset of the markers, where each marker is represented by
  two bits: 00 means that marker is not present, 01 means that the corresponding
  variable is being open (x𝈩), 10 means that it is being closed (𝈅x) and 11
  means that both opening and closing markers are in S. By being a long there is
  guaranteed enough bits for 16 variables.*/
  int i;
  Node* next;

  // NodeList replacement
  Node* start;
  Node* end;

  // Garbage Collection
  // TODO: Union this
  unsigned int refCount;
  Node* nextFree;

  Node(); // Class constructor
  Node(std::bitset<32> S_val, int, Node* head, Node* tail);
  Node *reset(std::bitset<32> S_val, int i_val, Node* head, Node* tail);
  Node *reset();
  void getNodeContent(int content[2]);
  bool isNodeEmpty();
};


class NodeList {
  public:
    Node* head;
    Node* tail;

    Node* it;

    NodeList(); // Class constructor

    void start();
    Node* next();
    bool empty();

    void add(Node*);
    void setHead(Node*);
    void setTail(Node*);
    void append(NodeList* list);
    void reset();
    void resetRefs();
    void resetAndAdd(Node*);
    std::string pprint(VariableFactory *vf);

    NodeList& operator=(const NodeList &nl);
};

#endif
