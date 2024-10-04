#include <bitset>
#include <iostream>
#include <sstream>

#include "output_enumeration/enumerator.hpp"

namespace REmatch {
inline namespace output_enumeration {

Enumerator::Enumerator() {
  current_mapping = new Mapping();
  amount_of_annotations_added_before_union.push_back(0);
}

Enumerator::~Enumerator() {
  delete current_mapping;
}

void Enumerator::reset() {
  stack.clear();
  amount_of_annotations_added_before_union.clear();
  amount_of_annotations_added_before_union.push_back(0);
  was_output_previously = false;
  current_mapping->delete_all_annotations();
}

const Mapping *Enumerator::next(){
  if (was_output_previously) {
    delete_annotations_until_previous_union();
    was_output_previously = false;
  }

  while (!stack.empty()) {
    ECSNode *current_node = stack.back();
    stack.pop_back();

    if (current_node->is_bottom()) {
      was_output_previously = true;
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

void Enumerator::delete_annotations_until_previous_union() {
  int amount_of_annotations_to_delete =
    amount_of_annotations_added_before_union.back();
  amount_of_annotations_added_before_union.pop_back();
  for (int i = 0; i < amount_of_annotations_to_delete; i++) {
    current_mapping->delete_previous_annotation();  // TODO: saltar para atrás en vez de borrar
  }
}

void Enumerator::consume_KLabel_node(ECSNode *current_kLabel_node) {
  current_mapping->add_annotations(current_kLabel_node->variable_markers,
                                  current_kLabel_node->document_position);
  stack.push_back(current_kLabel_node->next());
  amount_of_annotations_added_before_union.back()++;
}

void Enumerator::consume_union_node(ECSNode *current_union_node) {
  stack.push_back(current_union_node->right_node());
  stack.push_back(current_union_node->left_node());
  amount_of_annotations_added_before_union.push_back(0);
}

}
}
