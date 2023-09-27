#include "library_interface/match.hpp"
#include "match.hpp"

namespace REMatch {
inline namespace library_interface {

Match::Match(
    rematch::mediator::Mapping mapping,
    std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
    std::string_view document)
    : mapping_(mapping), variable_catalog_(variable_catalog), document_(document) {}

int Match::start(std::string variable_name) {
  Span span = this->span(variable_name);
  return span.first;
}

int Match::start(int variable_id) {
  return start(variable_catalog_->get_var(variable_id));
}

int Match::end(std::string variable_name) {
  Span span = this->span(variable_name);
  return span.second;
}

int Match::end(int variable_id) {
  return end(variable_catalog_->get_var(variable_id));
}

Span Match::span(std::string variable_name) {
  return (Span)mapping_.get_span_of_variable(variable_name);
}

Span Match::span(int variable_id) {
  return span(variable_catalog_->get_var(variable_id));
}

std::string Match::group(std::string variable_name) {
  Span span = this->span(variable_name);
  std::string_view group = document_.substr(span.first, span.second - span.first);
  return std::string(group);
}

std::string Match::group(int variable_id) {
  return group(variable_catalog_->get_var(variable_id));
}

std::map<std::string, Span> Match::groupdict() {
  return mapping_.get_spans_map();
}

bool Match::empty() {
  return mapping_.get_spans_map().size() == 0;
}

}
}  // namespace REMatch
