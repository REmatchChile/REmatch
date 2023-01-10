#ifndef STRUCTS__DAG__NODELIST_HPP
#define STRUCTS__DAG__NODELIST_HPP

#include <string>

//#include "factories/factories.hpp"

namespace rematch {

class NodeList {
 public:
  class Node;

  Node *head_;
  Node *tail_;

  NodeList(); // Class constructor

  bool empty();

  void add(Node *n);
  void set_head(Node *n);
  void set_tail(Node *n);
  void append(NodeList *list);
  void reset();
  void reset_refs();
  void reset_and_add(Node *n);

  NodeList &operator=(const NodeList &nl);
}; // end class NodeList

} // end namespace rematch

#endif // STRUCTS__DAG__NODELIST_HPP
