#ifndef OUTPUT_ENUMERATOR__ENUMERATION_HPP
#define OUTPUT_ENUMERATOR__ENUMERATION_HPP

#include "output_enumeration/ecs.hpp"
#include "output_enumeration/mapping.hpp"

namespace rematch {

class EndOfIterationException : public std::exception {
 public:
  const char * what() const throw () {
    return "Next was called on an empty enumerator";
  }
};

const int MAXIMUM_AMOUNT_OF_VARIABLES = 31;

class Enumerator {

 private:
  int amount_of_variables = MAXIMUM_AMOUNT_OF_VARIABLES;
  std::vector<ECSNode*> stack;
  Mapping current_mapping = Mapping(amount_of_variables);

 public:
  Enumerator() = default;
  Enumerator(int amount_of_variables);

  void add_node(ECSNode* n) { stack.push_back(n); };

  bool has_next() const { return !stack.empty(); }

  Mapping next();

 private:
  void consume_KLabel_node(ECSNode *current_kLabel_node);
  void consume_union_node(ECSNode *current_union_node);

};  // end class Enumerator

}  // end namespace rematch

#endif  // OUTPUT_ENUMERATOR__ENUMERATION_HPP
