#ifndef EVALUATION__SEGMENT_EVALUATOR_HPP
#define EVALUATION__SEGMENT_EVALUATOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "output_enumeration/mapping.hpp"

#include "segment_identification/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_state.hpp"

namespace rematch {

class SegmentIdentificator {

 public:
  SegmentIdentificator(
      LogicalVA& logical_va,
      std::string_view document
  );

  bool has_next();
  Span next();

 private:

  inline void reading(char a, int64_t i);

  SearchDFA search_dfa;
  std::string_view document;

  uint64_t i_src = 0;
  uint64_t i_min = 0;
  uint64_t i_max = 0;
}; // end class Evaluator

} // namespace rematch

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP
