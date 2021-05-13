#ifndef STRUCTS__DAG__NODE_HPP
#define STRUCTS__DAG__NODE_HPP

#include <bitset>

namespace rematch {

namespace internal {

class Node {
 private:
  static size_t ID;
 public:

  enum class Type {
    kBottom,
    kDummy,
  };

  size_t id_;
  std::bitset<32> S;
  /* S represents a subset of the markers, where each marker is represented by
  two bits: 00 means that marker is not present, 01 means that the corresponding
  variable is being open (x𝈩), 10 means that it is being closed (𝈅x) and 11
  means that both opening and closing markers are in S. By being a long there is
  guaranteed eno bits for 16 variables.*/
  int64_t i;
  Node* next;

  // NodeList replacement
  Node* start;
  Node* end;

  // Garbage Collection
  union {
    unsigned int refCount;
    Node* nextFree;
  };

  Node(Type t); // Class constructor
  Node(std::bitset<32> S_val, int64_t i, Node* head, Node* tail);
  Node *reset(std::bitset<32> S_val, int64_t i_val, Node* head, Node* tail);
  Node *reset();
  void getNodeContent(int content[2]);
  bool isNodeEmpty();
};

} // end namespace internal
} // end namespace rematch

#endif // STRUCTS__DAG__NODE_HPP