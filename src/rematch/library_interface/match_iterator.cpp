#include "library_interface/match_iterator.hpp"

namespace REMatch {
inline namespace library_interface {
MatchIterator::MatchIterator(
  rematch::SearchDFA &search_dfa, std::string_view &document
) :
  segment_identificator(
      rematch::SegmentIdentificator(search_dfa, document)
  ),
  document(document) {}

bool MatchIterator::has_next() {
  return segment_identificator.has_next();
}

Match MatchIterator::next() {
  return {segment_identificator.next(), document};
}

} // end namespace library_interface
} // end namespace rematch
