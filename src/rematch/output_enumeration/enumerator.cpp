#include <bitset>
#include <iostream>
#include <sstream>

#include "output_enumeration/enumerator.hpp"

namespace rematch {

Enumerator::Enumerator(int amount_of_variables)
    : current_mapping(amount_of_variables * 2) {}

Mapping Enumerator::next() {
  while (!stack_.empty()) {

    auto current_node = stack_.back();
    stack_.pop_back();

    if (current_node->is_bottom()) {
      tot_mappings_++;
      return current_mapping;
    } else if (current_node->is_output()) {  // If label node
      current_mapping.add_annotations(current_node->variable_markers,
                                      current_node->document_position);
      stack_.push_back(current_node->next());
    } else { // if union node
      stack_.push_back(current_node->right_node());
      stack_.push_back(current_node->left_node());
    }
  }

  throw std::exception();
}

}  // end namespace rematch
