#include "match.hpp"

#include "evaluation/document.hpp"

using namespace rematch;

namespace REMatch {
inline namespace library_interface {

Match::Match(
    rematch::mediator::Mapping mapping,
    std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
    std::shared_ptr<Document> document)
    : mapping_(mapping),
      variable_catalog_(variable_catalog),
      document_(document) {}

int Match::start(std::string variable_name) {
  auto const span = this->span(variable_name);
  return span.first;
}

int Match::start(int variable_id) {
  return start(variable_catalog_->get_var(variable_id));
}

int Match::end(std::string variable_name) {
  auto const span = this->span(variable_name);
  return span.second;
}

int Match::end(int variable_id) {
  return end(variable_catalog_->get_var(variable_id));
}

Span Match::span(std::string variable_name) {
  return mapping_.get_span_of_variable(variable_name);
}

Span Match::span(int variable_id) {
  return span(variable_catalog_->get_var(variable_id));
}

std::string Match::group(std::string variable_name) {
  const auto span = this->span(variable_name);
  return document_->substr(span.first, span.second - span.first);
}

std::string Match::group(int variable_id) {
  return group(variable_catalog_->get_var(variable_id));
}

std::map<std::string, Span> Match::groupdict() {
  return mapping_.get_spans_map();
}

std::vector<std::string> Match::variables() {
  if (empty())
    return {};

  return variable_catalog_->variables();
}

bool Match::empty() {
  return mapping_.get_spans_map().size() == 0;
}

std::ostream& operator<<(std::ostream& os, Match& match) {
  int amount_of_variables = match.variable_catalog_->size();

  for (unsigned int i = 0; i < match.variable_catalog_->size() - 1; i++) {
    std::string variable_name = match.variable_catalog_->get_var(i);
    Span span = match.span(variable_name);
    os << variable_name << " = |" << span.first << "," << span.second << ">\t";
  }

  std::string variable_name = match.variable_catalog_->get_var(amount_of_variables - 1);
  Span span = match.span(variable_name);
  os << variable_name << " = |" << span.first << "," << span.second << ">";

  return os;
}

}  // namespace library_interface
}  // namespace REMatch
