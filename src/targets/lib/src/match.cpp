#include <REmatch/match.hpp>
#include <cstdint>

#include "evaluation/document.hpp"

namespace REmatch {
inline namespace library_interface {

Match::Match(mediator::Mapping mapping,
             std::shared_ptr<parsing::VariableCatalog> variable_catalog,
             std::shared_ptr<Document> document)
    : mapping_(mapping),
      variable_catalog_(variable_catalog),
      document_(document) {}

int64_t Match::start(const std::string& variable_name) {
  return this->span(variable_name).first;
}

int64_t Match::start(uint_fast32_t variable_id) {
  return start(variable_catalog_->get_var(variable_id));
}

int64_t Match::end(const std::string& variable_name) {
  const auto span = this->span(variable_name);
  return span.second;
}

int64_t Match::end(uint_fast32_t variable_id) {
  return end(variable_catalog_->get_var(variable_id));
}

Span Match::span(const std::string& variable_name) {
  return mapping_.get_span_of_variable(variable_name);
}

Span Match::span(uint_fast32_t variable_id) {
  return span(variable_catalog_->get_var(variable_id));
}

std::string Match::group(const std::string& variable_name) {
  const auto span = this->span(variable_name);
  return document_->substr(span.first, span.second - span.first);
}

std::string Match::group(uint_fast32_t variable_id) {
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
  return mapping_.get_spans_map().empty();
}

std::ostream& operator<<(std::ostream& os, Match& match) {
  const auto num_variables = match.variable_catalog_->size();

  for (unsigned int i = 0; i < num_variables - 1; i++) {
    const auto variable_name = match.variable_catalog_->get_var(i);
    const auto span = match.span(variable_name);
    os << variable_name << " = |" << span.first << "," << span.second << ">\t";
  }

  const auto variable_name =
      match.variable_catalog_->get_var(num_variables - 1);
  const auto span = match.span(variable_name);
  os << variable_name << " = |" << span.first << "," << span.second << ">";

  return os;
}

}  // namespace library_interface
}  // namespace REmatch
