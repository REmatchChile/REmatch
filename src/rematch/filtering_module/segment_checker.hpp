#pragma once

#include <memory>

#include "REmatch/span.hpp"
#include "evaluation/document.hpp"
#include "evaluation/start_end_chars.hpp"
#include "evaluation/stream.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"

namespace REmatch {
class SegmentChecker {

 public:
  SegmentChecker(std::unique_ptr<SearchDFA> search_dfa, std::shared_ptr<Document> document)
      : search_dfa(std::move(search_dfa)), document(std::move(document)) {}

  bool check_(Span span) {
    uint64_t i_src = span.first;
    uint64_t doc_end_i_ = span.second;

    for (; i_src < doc_end_i_; i_src++) {
      char a = (*document)[i_src];

      SearchDFAState* current_state = search_dfa->next_state(a);

      if (current_state->accepting()) {
        return true;
      }
    }
    return false;
  }

  bool check(Span span) {
    search_dfa->next_state(START_CHAR);

    if (check_(span)) {
      return true;
    }
    SearchDFAState* state = search_dfa->next_state(END_CHAR);
    return state->accepting();
  }

 private:
  std::unique_ptr<SearchDFA> search_dfa;
  std::shared_ptr<Document> document;
};

class SegmentCheckerStream {
 public:
  SegmentCheckerStream(std::unique_ptr<SearchDFA> search_dfa, std::shared_ptr<Stream> stream)
      : search_dfa(std::move(search_dfa)), stream(std::move(stream)) {}

  bool check_(Span span) {
    uint64_t i_src = span.first;
    uint64_t doc_end_i_ = span.second;

    char a;
    for (; i_src < doc_end_i_; i_src++) {
      stream->read_back(a, i_src);

      SearchDFAState* current_state = search_dfa->next_state(a);

      if (current_state->accepting()) {
        return true;
      }
    }
    return false;
  }

  bool check(Span span) {
    search_dfa->next_state(START_CHAR);

    if (check_(span)) {
      return true;
    }
    SearchDFAState* state = search_dfa->next_state(END_CHAR);
    return state->accepting();
  }

  bool check() {
    search_dfa->next_state(START_CHAR);

    char a;
    while (stream->read(a)) {
      SearchDFAState* current_state = search_dfa->next_state(a);

      if (current_state->accepting()) {
        return true;
      }
    }

    SearchDFAState* state = search_dfa->next_state(END_CHAR);
    return state->accepting();
  }

 private:
  std::unique_ptr<SearchDFA> search_dfa;
  std::shared_ptr<Stream> stream;
};

}  // namespace REmatch
