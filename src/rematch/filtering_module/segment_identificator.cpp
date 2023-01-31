#include <iostream>

#include "filtering_module/segment_identificator.hpp"

namespace rematch {
inline namespace filtering_module {
SegmentIdentificator::SegmentIdentificator(
    SearchDFA &search_dfa, std::string_view document
  ) : search_dfa(search_dfa),
      document(document) {
      search_dfa.reset();
    }

bool SegmentIdentificator::has_next() {
  i_min = i_src;
  i_max = i_src;

  for (; i_src < document.size(); i_src++) {

    char a = (char) document[i_src];

    SearchDFAState* current_state = search_dfa.next_state(a);

    if (current_state->accepting()) {
      i_max = i_src + 1;
  }
    else if (current_state->ends()) {
      if (i_min < i_max) {
        return true;
      }
      if (current_state->empty_subset())
        i_min = i_src + 1;
      else
       i_min = i_src;
    }
  }

  if (i_min < i_max) {
    return true;
  }

  i_min = document.size();

  return false;
}

Span SegmentIdentificator::next() {
  return {i_min, i_max};
}

}
}
