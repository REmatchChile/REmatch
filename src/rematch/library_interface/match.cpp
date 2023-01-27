#include "library_interface/match.hpp"

namespace REMatch {
inline namespace library_interface {

Match::Match(Span span, std::string_view document) :
  span(span), document(document) {}

Span Match::get_span() const {
  return span;
}

std::string_view Match::to_string() const {
  return document.substr(span.first, span.second - span.first);
}

}
}
