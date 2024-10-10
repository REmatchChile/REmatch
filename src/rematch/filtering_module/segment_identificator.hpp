#ifndef EVALUATION__SEGMENT_EVALUATOR_HPP
#define EVALUATION__SEGMENT_EVALUATOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "output_enumeration/mapping.hpp"

#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa_state.hpp"

#include <REmatch/span.hpp>

namespace REmatch {
class Document;

inline namespace filtering_module {

/**
  * The responsability of the SegmentIdentificator class is the same one
  * that the 'filering module' has on the paper:
  * REmatch: a regex engine for finding all matches
  * (Riveros, Van Sint Jan, Vrgoc 2023).
  *
  * The SegmentIdentificator identifies through has_next() and next()
  * the next segment of the document that has at least one output.
  */
class SegmentIdentificator {

 public:
  SegmentIdentificator(SearchDFA& search_dfa, std::shared_ptr<Document> document);

  /**
   * next_is_computed_successfully() MUST be called before, if not, next() has undefined
   * behavior.
   */
  std::unique_ptr<Span> next();
  void set_document_indexes(Span& span);
  size_t get_search_dfa_size();
  size_t get_search_nfa_size();

 private:
  /**
   * Tries to compute the next span that has an output
   * if it is computed correctly, next() will return that span
   * and the method will return true. If not, this method will
   * return false and next() will return nullptr.
   */
  bool next_is_computed_successfully();

  SearchDFA& search_dfa;
  std::shared_ptr<Document> document;

  uint64_t doc_start_i_ = 0;
  uint64_t doc_end_i_ = 0;

  uint64_t i_src = 0;
  uint64_t i_min = 0;
  uint64_t i_max = 0;
};

}
}

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP
