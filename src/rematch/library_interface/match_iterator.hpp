#ifndef MATCH_ITERATOR__REMATCH_HPP
#define MATCH_ITERATOR__REMATCH_HPP

#include <string_view>

#include "library_interface/match.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"

namespace REMatch {

inline namespace library_interface {
  class MatchIterator {

  private:
    rematch::SegmentIdentificator segment_identificator;
    std::string_view &document;

  public:
    MatchIterator(rematch::SearchDFA &search_dfa, std::string_view &document);
    bool has_next();
    Match next();
  };


} // end namespace library_interface
} // end namespace rematch

#endif
