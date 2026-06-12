#include "match_standard.hpp"
#include "REmatch/exceptions.hpp"

namespace REmatch::internal {

MatchStandard::MatchStandard(std::unique_ptr<mediator::Mapping> mapping,
                             std::shared_ptr<parsing::VariableCatalog> variable_catalog,
                             std::shared_ptr<Document> document)
    : mapping_(std::move(mapping)),
      variable_catalog_(std::move(variable_catalog)),
      document_(std::move(document)),
      num_variables(variable_catalog_->size()) {}

MatchStandard::MatchStandard(const MatchStandard& other)
    : mapping_(std::make_unique<mediator::Mapping>(*other.mapping_)),
      variable_catalog_(other.variable_catalog_),
      document_(other.document_),
      num_variables(other.num_variables) {}

MatchStandard& MatchStandard::operator=(const MatchStandard& other) {
  if (this == &other) {
    return *this;
  }

  mapping_ = std::make_unique<mediator::Mapping>(*other.mapping_);
  variable_catalog_ = other.variable_catalog_;
  document_ = other.document_;
  num_variables = other.num_variables;
  return *this;
}

MatchStandard::MatchStandard(MatchStandard&& other) noexcept
    : mapping_(std::move(other.mapping_)),
      variable_catalog_(std::move(other.variable_catalog_)),
      document_(std::move(other.document_)),
      num_variables(other.num_variables) {}

MatchStandard& MatchStandard::operator=(MatchStandard&& other) noexcept {
  mapping_ = std::move(other.mapping_);
  variable_catalog_ = std::move(other.variable_catalog_);
  document_ = std::move(other.document_);
  num_variables = other.num_variables;
  return *this;
}

MatchStandard::~MatchStandard() = default;

int64_t MatchStandard::start(const std::string& variable_name) const {
  return this->span(variable_name).first;
}

int64_t MatchStandard::start(uint_fast32_t variable_id) const {
  if (variable_id >= num_variables) {
    throw VariableNotFoundException("Variable id '" + std::to_string(variable_id) +
                                    "' is out of range");
  }
  return start(variable_catalog_->get_var(variable_id));
}

int64_t MatchStandard::end(const std::string& variable_name) const {
  const auto span = this->span(variable_name);
  return span.second;
}

int64_t MatchStandard::end(uint_fast32_t variable_id) const {
  if (variable_id >= num_variables) {
    throw VariableNotFoundException("Variable id '" + std::to_string(variable_id) +
                                    "' is out of range");
  }
  return end(variable_catalog_->get_var(variable_id));
}

Span MatchStandard::span(const std::string& variable_name) const {
  int variable_id = variable_catalog_->position(variable_name);
  return mapping_->get_span_of_variable(variable_id);
}

Span MatchStandard::span(uint_fast32_t variable_id) const {
  if (variable_id >= num_variables) {
    throw VariableNotFoundException("Variable id '" + std::to_string(variable_id) +
                                    "' is out of range");
  }
  return span(variable_catalog_->get_var(variable_id));
}

std::string MatchStandard::group(const std::string& variable_name) const {
  const auto span = this->span(variable_name);
  return document_->substr(span.first, span.second - span.first);
}

std::string MatchStandard::group(uint_fast32_t variable_id) const {
  if (variable_id >= num_variables) {
    throw VariableNotFoundException("Variable id '" + std::to_string(variable_id) +
                                    "' is out of range");
  }
  return group(variable_catalog_->get_var(variable_id));
}

std::map<std::string, Span> MatchStandard::groupdict() const {
  auto id_to_span = mapping_->get_spans_map();
  std::map<std::string, Span> group_to_span;

  for (auto&& [var_id, span] : id_to_span) {
    group_to_span[variable_catalog_->get_var(var_id)] = span;
  }

  return group_to_span;
}

std::vector<std::string> MatchStandard::variables() const {
  return variable_catalog_->variables();
}

bool MatchStandard::empty() const {
  return mapping_->get_spans_map().empty();
}

std::string MatchStandard::to_string() const {
  std::stringstream ss;

  if (num_variables == 0) {
    return "{}";
  }

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

}  // namespace REmatch::internal
