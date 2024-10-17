#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "library_interface/reader.hpp"
#include "output_enumeration/mapping.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"

#include "evaluation/document.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa_state.hpp"

namespace rematch {

inline namespace filtering_module {

class SegmentIdentificatorStream {

 public:
  SegmentIdentificatorStream(SearchDFA& search_dfa,
                             std::shared_ptr<Reader> reader)
      : search_dfa(search_dfa), reader_(std::move(reader)) {
    search_dfa.reset();
    keep = reader_->read(current_char);
  }

  bool next_is_computed_successfully() {
    // reader_->seek(static_cast<int64_t>(i_max));

    i_min = i_src;
    i_max = i_src;

    while (keep) {
      SearchDFAState* current_state = search_dfa.next_state(current_char);

      if (current_state->accepting()) {
        i_max = i_src + 1;
      } else if (current_state->ends()) {
        if (i_min < i_max) {
          return true;
        }
        if (current_state->empty_subset())
          i_min = i_src + 1;
        else
          i_min = i_src;
      }

      i_src++;
      keep = reader_->read(current_char);
    }

    if (i_min < i_max) {
      return true;
    }

    i_min = i_src;  //

    return false;
  }

  std::unique_ptr<Span> next() {
    if (!next_is_computed_successfully()) {
      return nullptr;
    }
    return std::make_unique<Span>(i_min, i_max);
  }

  size_t get_search_dfa_size() const { return search_dfa.states.size(); }

  size_t get_search_nfa_size() { return search_dfa.get_search_nfa_size(); }

  SearchDFA& search_dfa;
  std::shared_ptr<Reader> reader_;

  uint64_t doc_start_i_ = 0;
  uint64_t doc_end_i_ = 0;

  uint64_t i_src = 0;
  uint64_t i_min = 0;

  // TODO:
  uint64_t i_max = 1;

  bool keep = true;
  char current_char{};
};

}  // namespace filtering_module
}  // namespace rematch
