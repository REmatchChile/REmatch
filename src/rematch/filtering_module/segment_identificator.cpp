#include "filtering_module/segment_identificator.hpp"

#include "evaluation/document.hpp"
#include "evaluation/start_end_chars.hpp"
#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {
inline namespace filtering_module {

SegmentIdentificator::SegmentIdentificator(std::unique_ptr<SearchDFA> search_dfa,
                                           std::shared_ptr<Document> document)
    : search_dfa(std::move(search_dfa)), document(document), doc_end_i_(document->size()) {
  search_dfa.reset();
}

bool SegmentIdentificator::next_is_computed_successfully() {
  i_min = i_src;
  i_max = i_src;

  for (; i_src < doc_end_i_; i_src++) {

    char a = (*document)[i_src];

    SearchDFAState* current_state = search_dfa->next_state(a);

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
  }

  if (i_min < i_max) {
    return true;
  }

  i_min = document->size();

  return false;
}

std::unique_ptr<Span> SegmentIdentificator::next() {
#ifdef TRACY_ENABLE
  ZoneScopedNC("SegmentIdentificator::next", 0x800080);
#endif

  if (!next_is_computed_successfully()) {
    return nullptr;
  }
  return std::make_unique<Span>(i_min, i_max);
}

void SegmentIdentificator::evaluate_start_char() {
  search_dfa->next_state(START_CHAR);
}

void SegmentIdentificator::set_document_indexes(Span& span) {
  doc_start_i_ = span.first;
  doc_end_i_ = span.second;
  i_min = doc_start_i_;
  i_max = doc_start_i_;
  i_src = doc_start_i_;
  search_dfa.reset();
}

size_t SegmentIdentificator::get_search_dfa_size() const {
  return search_dfa->states.size();
}

size_t SegmentIdentificator::get_search_nfa_size() const {
  return search_dfa->get_search_nfa_size();
}

}  // namespace filtering_module
}  // namespace REmatch
