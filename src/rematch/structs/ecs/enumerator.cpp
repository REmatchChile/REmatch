#include "enumerator.hpp"

#include <bitset>
#include <iostream>
#include <sstream>

#include "factories/factories.hpp"
#include "match.hpp"
#include "regex/regex.hpp"
#include "structs/dag/fastnodelist.hpp"
#include "structs/dag/node.hpp"
#include "structs/dag/nodelist.hpp"

namespace rematch {

Enumerator::Enumerator(RegEx &r)
    : var_factory_(r.vfactory()),
      current_mapping_(var_factory_->size() * 2, -1) {}

Match_ptr Enumerator::next() {
  while (!stack_.empty()) {
    auto current_node = stack_.back();

    stack_.pop_back();

    if (current_node->is_bottom()) {
      tot_mappings_++;
      std::unique_ptr<Match> ret(new Match(var_factory_, current_mapping_));
      return ret;
    }

    if (current_node->is_output()) { // If label node
      ECS::Data dt = current_node->data();
      for (size_t j = 0; j < var_factory_->size() * 2; j++) {
        if (dt.S[j])
          current_mapping_[j] = dt.i - var_factory_->get_offset(j);
        // TODO: Hacer una version con/sin offset dependiendo del automata
      }
      stack_.push_back(current_node->next());
    } else { // If union node
      stack_.push_back(current_node->right());
      stack_.push_back(current_node->left());
    }
  } // end while()

  throw std::exception();
}

} // end namespace rematch
