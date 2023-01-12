#ifndef ECS__ENUMERATION_HPP
#define ECS__ENUMERATION_HPP

#include "output_enumeration/ecs.hpp"
#include "output_enumeration/mapping.hpp"

namespace rematch {

// Interface for enumerating the DAG-like structure obtained after
// the evaluation step (preprocessing the ourput). It uses the common
// methods next() and hasNext() for obtaining the outputs.
  const int MAXIMUM_AMOUNT_OF_VARIABLES = 31;

class Enumerator {

 private:
  int amount_of_variables = MAXIMUM_AMOUNT_OF_VARIABLES;
  std::vector<ECSNode*> stack_;
  Mapping current_mapping;

  uint64_t tot_mappings_ = 0;

  std::string_view document_;

 public:
  Enumerator() = default;
  Enumerator(int amount_of_variables);

  void add_node(ECSNode* n) { stack_.push_back(n); };

  bool has_next() const { return !stack_.empty(); }

  Mapping next();

};  // end class Enumerator

}  // end namespace rematch

#endif  // ECS__ENUMERATION_HPP
