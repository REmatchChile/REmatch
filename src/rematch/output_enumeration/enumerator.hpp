#ifndef OUTPUT_ENUMERATOR__ENUMERATION_HPP
#define OUTPUT_ENUMERATOR__ENUMERATION_HPP

#include "output_enumeration/ecs.hpp"
#include "output_enumeration/mapping.hpp"

namespace REmatch {

inline namespace output_enumeration {

class EndOfIterationException : public std::exception {
 public:
  const char * what() const throw () {
    return "Next was called on an empty enumerator";
  }
};

const int MAXIMUM_AMOUNT_OF_VARIABLES = 31;

class Enumerator {

 private:
  std::vector<ECSNode*> stack = {};
  Mapping *current_mapping;

  bool was_output_previously = false;
  std::vector<int> amount_of_annotations_added_before_union = {};

 public:
  Enumerator();
  Enumerator(const Enumerator&) = delete;
  ~Enumerator();

  void reset();
  void add_node(ECSNode* n) { stack.push_back(n); };

  bool has_next() const { return !stack.empty(); }
  const Mapping *next();

 private:
  void delete_annotations_until_previous_union();
  void consume_KLabel_node(ECSNode *current_kLabel_node);
  void consume_union_node(ECSNode *current_union_node);

};
}
}

#endif
