#ifndef STRUCTS__ECS__ENUMERATION_HPP
#define STRUCTS__ECS__ENUMERATION_HPP

#include <bitset>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "factories/factories.hpp"
#include "match.hpp"
#include "structs/dag/fastnodelist.hpp"
#include "structs/dag/node.hpp"
#include "structs/dag/nodelist.hpp"
#include "structs/ecs/ecs.hpp"

namespace rematch {

class RegEx;

// Interface for enumerating the DAG-like structure obtained after
// the evaluation step (preprocessing the ourput). It uses the common
// methods next() and hasNext() for obtaining the outputs.
class Enumerator {
 public:
  Enumerator(RegEx& r, std::string_view document);

  void add_node(ECS::Node* n) { stack_.push_back(n); };

  bool has_next() const { return !stack_.empty(); }

  Match_ptr next();

 private:
  // Reference to Variable Factory
  std::shared_ptr<VariableFactory> var_factory_;

  // Depth Stack
  std::vector<ECS::Node*> stack_;
  std::vector<int64_t> current_mapping_;

  uint64_t tot_mappings_ = 0;

  std::string_view document_;
};  // end class Enumerator

}  // end namespace rematch

#endif  // STRUCTS__ECS__ENUMERATION_HPP