#include "library_interface/regex.hpp"
#include "parsing/parser.hpp"

namespace REMatch {
inline namespace library_interface {

Regex::Regex(std::string_view pattern, Flags flags) {
  rematch::Parser parser = rematch::Parser(pattern);
  rematch::LogicalVA logical_va = parser.get_logical_va();
  search_dfa = new rematch::SearchDFA(logical_va);
}

Regex::~Regex() {
  delete search_dfa;
}

std::optional<Match> Regex::find(std::string_view text, Flags flags) {
  MatchIterator iterator = finditer(text, flags);
  if (iterator.has_next()) {
    return {iterator.next()};
  }
  return {};
}

MatchIterator Regex::finditer(std::string_view text, Flags flags) {
  return {*search_dfa, text};
}

} // end namespace library_interface
} // end namespace rematch
