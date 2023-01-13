#include <bitset>
#include <iostream>
#include <sstream>

#include "output_enumeration/enumerator.hpp"

namespace rematch {

Enumerator::Enumerator(int amount_of_variables)
    : current_mapping(amount_of_variables * 2) {}

Mapping Enumerator::next() {
  while (!stack.empty()) {
    ECSNode *current_node = stack.back();
    stack.pop_back();

    if (current_node->is_bottom()) {
      return current_mapping;
    } else if (current_node->is_output()) {
        consume_KLabel_node(current_node);
    } else {
        consume_union_node(current_node);
    }
  }
  throw EndOfIterationException();
  return current_mapping; // to disable warning about missing return statement
}

void Enumerator::consume_KLabel_node(ECSNode *current_kLabel_node) {
  current_mapping.add_annotations(current_kLabel_node->variable_markers,
                                  current_kLabel_node->document_position);
  stack.push_back(current_kLabel_node->next());
}

void Enumerator::consume_union_node(ECSNode *current_union_node) {
  stack.push_back(current_union_node->right_node());
  stack.push_back(current_union_node->left_node());
}

}  // end namespace rematch
