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

    char a = (char)(document[i_src] & 0x7F); // remove 0x7F?

    SearchDFAState* current_state = search_dfa.next_state(a);

    if (current_state->accepting())
      i_max = i_src + 1;
    else if (current_state->ends()) {
      if (i_min < i_max) {
        i_src++;
        return true;
      }
      i_min = i_src + 1;
    }
  }

  if (i_min < i_max) {
    search_dfa.reset();
    i_src++;
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
