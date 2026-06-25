#pragma once

#include <memory>
#include <string>
#include <vector>

#include "REmatch/fstream_reader.hpp"
#include "evaluation/stream.hpp"
#include "output_enumeration/mapping.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"

#include "evaluation/document.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa_state.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {

enum SegmentResult { NoSegmentFound, FitsInBuffer, DoesNotFitInBuffer };

inline namespace filtering_module {

class SegmentIdentificatorStream {

 public:
  SegmentIdentificatorStream(std::unique_ptr<SearchDFA> search_dfa, std::shared_ptr<Stream> stream)
      : search_dfa(std::move(search_dfa)),
        stream_(std::move(stream)),
        buffer_limit(stream_->get_buffer_size() - 1) {
    search_dfa.reset();
    read_from_stream = stream_->read(current_char);
  }

  SegmentResult next_is_computed_successfully() {
    i_min = i_src;
    i_max = i_src;

    while (read_from_stream) {
      SearchDFAState* current_state = search_dfa->next_state(current_char);

      if (current_state->accepting()) {
        i_max = i_src + 1;
      } else if (current_state->ends()) {
        if (i_min < i_max) {
          return FitsInBuffer;
        }

        // if empty subset: i_min = i_src + 1
        // otherwise:       i_min = i_src
        i_min = i_src + static_cast<int>(current_state->empty_subset());
      }

      if (i_src - i_min >= buffer_limit && i_min < i_src) {
        i_max = i_src;
        return DoesNotFitInBuffer;
      }
      i_src++;
      read_from_stream = stream_->read(current_char);
    }

    if (i_min < i_src) {
      return FitsInBuffer;
    }

    i_min = i_src;
    return NoSegmentFound;
  }

  std::pair<std::unique_ptr<Span>, SegmentResult> next() {
#ifdef TRACY_ENABLE
    ZoneScopedNC("SegmentIdentificator::next", 0xd79921);
#endif
    SegmentResult segment_result = next_is_computed_successfully();

    return {std::make_unique<Span>(i_min, i_max), segment_result};
  }

  size_t get_search_dfa_size() const { return search_dfa->states.size(); }

  size_t get_search_nfa_size() { return search_dfa->get_search_nfa_size(); }

  void evaluate_start_char() { search_dfa->next_state(START_CHAR); }

 private:
  std::unique_ptr<SearchDFA> search_dfa;
  std::shared_ptr<Stream> stream_;
  const uint64_t buffer_limit;

  uint64_t i_src = 0;
  uint64_t i_min = 0;
  uint64_t i_max = 1;

  char current_char{};
  bool read_from_stream;
};

}  // namespace filtering_module
}  // namespace REmatch
