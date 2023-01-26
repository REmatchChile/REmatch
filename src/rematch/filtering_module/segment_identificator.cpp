#include <iostream>

#include "filtering_module/segment_identificator.hpp"

#define	FORCE_INLINE inline __attribute__((always_inline))

// Anchor?

namespace rematch {
SegmentIdentificator::SegmentIdentificator(
    LogicalVA& logical_va, std::string_view document
  ) : search_dfa(SearchDFA(logical_va)),
      document(document) {}

bool SegmentIdentificator::has_next() {
  /**
   * Tries to compute the next span that has an output
   * if it is computed correctly, next() will return that span
   * and the method will return true. If not, this method will
   * return false and next() will return an arbitrary span.
   */
  i_min = i_src;
  i_max = i_src;

  for (; i_src < document.size(); i_src++) {

    char a = (char) document[i_src];

    //std::cout << "pos: " << i_src << " reading: " << (int) ((uint8_t) a) << std::endl;
    SearchDFAState* current_state = search_dfa.next_state(a);

    if (current_state->accepting()) {
      //std::cout << "accepting" << std::endl;
      i_max = i_src + 1;
  }
    else if (current_state->ends()) {
      //std::cout << "ends" << std::endl;
      if (i_min < i_max) {
        return true;
      }
      if (current_state->empty_subset())
        i_min = i_src + 1; // Added +1
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
  /**
   * This method returns the current span in the document that has
   * output(s). It MUST be called after has_next().
   */
  return {i_min, i_max};
}

}
