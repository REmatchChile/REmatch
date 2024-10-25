#include <REmatch/match.hpp>

#include <REmatch/REmatch_export.hpp>

#include <cstdint>
#include <memory>
#include <sstream>

#include "evaluation/document.hpp"
#include "mediator/mapping.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {
inline namespace library_interface {

Match::Match(std::unique_ptr<mediator::Mapping> mapping,
             std::shared_ptr<parsing::VariableCatalog> variable_catalog,
             std::shared_ptr<Document> document)
    : mapping_(std::move(mapping)),
      variable_catalog_(variable_catalog),
      document_(document) {}

Match::Match(const Match& other)
    : mapping_(std::make_unique<mediator::Mapping>(*other.mapping_)),
      variable_catalog_(other.variable_catalog_),
      document_(other.document_) {}

Match& Match::operator=(const Match& other) {
  if (this == &other) {
    return *this;
  }

  mapping_ = std::make_unique<mediator::Mapping>(*other.mapping_);
  variable_catalog_ = other.variable_catalog_;
  document_ = other.document_;
  return *this;
}

Match::Match(Match&& other) noexcept
    : mapping_(std::move(other.mapping_)),
      variable_catalog_(std::move(other.variable_catalog_)),
      document_(std::move(other.document_)) {}

Match& Match::operator=(Match&& other) noexcept {
  mapping_ = std::move(other.mapping_);
  variable_catalog_ = std::move(other.variable_catalog_);
  document_ = std::move(other.document_);
  return *this;
}

Match::~Match() = default;

int64_t Match::start(const std::string& variable_name) const {
  return this->span(variable_name).first;
}

int64_t Match::start(uint_fast32_t variable_id) const {
  return start(variable_catalog_->get_var(variable_id));
}

int64_t Match::end(const std::string& variable_name) const {
  const auto span = this->span(variable_name);
  return span.second;
}

int64_t Match::end(uint_fast32_t variable_id) const {
  return end(variable_catalog_->get_var(variable_id));
}

Span Match::span(const std::string& variable_name) const {
  return mapping_->get_span_of_variable(variable_name);
}

Span Match::span(uint_fast32_t variable_id) const {
  return span(variable_catalog_->get_var(variable_id));
}

std::string Match::group(const std::string& variable_name) const {
  const auto span = this->span(variable_name);
  return document_->substr(span.first, span.second - span.first);
}

std::string Match::group(uint_fast32_t variable_id) const {
  return group(variable_catalog_->get_var(variable_id));
}

std::map<std::string, Span> Match::groupdict() const {
  return mapping_->get_spans_map();
}

std::vector<std::string> Match::variables() const {
  return variable_catalog_->variables();
}

bool Match::empty() const {
  return mapping_->get_spans_map().empty();
}

std::string Match::to_string() const {
  std::stringstream ss;

  const auto num_variables = variable_catalog_->size();

  ss << "{";
  for (unsigned int i = 0; i < num_variables - 1; i++) {
    const auto variable_name = variable_catalog_->get_var(i);
    const auto span_ = span(variable_name);
    ss << variable_name << ": |" << span_.first << "," << span_.second << ">, ";
  }

  const auto variable_name = variable_catalog_->get_var(num_variables - 1);
  const auto span_ = span(variable_name);
  ss << variable_name << ": |" << span_.first << "," << span_.second << ">}";

  return ss.str();
}

REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const Match& match) {
  return os << match.to_string();
}

}  // namespace library_interface
}  // namespace REmatch
