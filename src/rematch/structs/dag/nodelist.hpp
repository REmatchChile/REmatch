#ifndef STRUCTS__DAG__NODELIST_HPP
#define STRUCTS__DAG__NODELIST_HPP

#include <string>

#include "factories/factories.hpp"
#include "structs/dag/node.hpp"

namespace rematch {
namespace internal {

class NodeList {
public:
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

} // end namespace internal
} // end namespace rematch

#endif // STRUCTS__DAG__NODELIST_HPP