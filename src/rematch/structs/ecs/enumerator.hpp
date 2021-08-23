#ifndef STRUCTS__ECS__ENUMERATION_HPP
#define STRUCTS__ECS__ENUMERATION_HPP

#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#include <string>
#include <stack>
#include <utility>
#include <memory>

#include "match.hpp"
#include "structs/dag/nodelist.hpp"
#include "structs/dag/fastnodelist.hpp"
#include "structs/dag/node.hpp"
#include "structs/ecs/ecs.hpp"
#include "factories/factories.hpp"
#include "memmanager.hpp"

namespace rematch {

class RegEx;

namespace internal {

// Interface for enumerating the DAG-like structure obtained after
// the evaluation step (preprocessing the ourput). It uses the common
// methods next() and hasNext() for obtaining the outputs.
class Enumerator {
 public:
  Enumerator(RegEx &r);

  void add_node(internal::ECS::Node* n) { stack_.push_back(n); };

  bool has_next() const { return !stack_.empty(); }

  Match_ptr next();

  // TODO: Implement this
  void next(Match *m);

 private:
  // Reference to Variable Factory
  std::shared_ptr<VariableFactory> var_factory_;

  // Depth Stack
  std::vector<internal::ECS::Node*> stack_;
  std::vector<int64_t> current_mapping_;

  uint64_t tot_mappings_ = 0;
}; // end class Enumerator

} // end namespace internal
} // end namespace rematch


#endif // STRUCTS__ECS__ENUMERATION_HPP