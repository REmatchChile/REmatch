#include <REmatch/s_match.hpp>

#include <REmatch/REmatch_export.hpp>

#include "evaluation/stream.hpp"
#include "mediator/mapping.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {
inline namespace library_interface {

SMatch::SMatch(std::unique_ptr<mediator::Mapping> mapping,
               std::shared_ptr<VariableCatalog> variable_catalog, std::shared_ptr<Stream> stream)
    : mapping_(std::move(mapping)),
      variable_catalog_(std::move(variable_catalog)),
      stream(std::move(stream)) {}

SMatch::~SMatch() = default;

int64_t SMatch::start(const std::string& variable_name) const {
  return this->span(variable_name).first;
}

int64_t SMatch::start(uint_fast32_t variable_id) const {
  return start(variable_catalog_->get_var(variable_id));
}

int64_t SMatch::end(const std::string& variable_name) const {
  const auto span = this->span(variable_name);
  return span.second;
}

int64_t SMatch::end(uint_fast32_t variable_id) const {
  return end(variable_catalog_->get_var(variable_id));
}

Span SMatch::span(const std::string& variable_name) const {
  return mapping_->get_span_of_variable(variable_catalog_->position(variable_name));
}

Span SMatch::span(uint_fast32_t variable_id) const {
  return span(variable_catalog_->get_var(variable_id));
}

std::string SMatch::group(const std::string& variable_name) const {
  Span span_var = span(variable_name);
  return stream->substr(span_var);
}

std::string SMatch::group(uint_fast32_t variable_id) const {
  return group(variable_catalog_->get_var(variable_id));
}

std::string SMatch::to_string() const {
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

std::vector<std::string> SMatch::variables() const {
  return variable_catalog_->variables();
}

std::map<std::string, Span> SMatch::groupdict() const {
  auto id_to_span = mapping_->get_spans_map();
  std::map<std::string, Span> group_to_span;

  for (auto&& [var_id, span] : id_to_span) {
    group_to_span[variable_catalog_->get_var(var_id)] = span;
  }

  return group_to_span;
}

bool SMatch::empty() const {
  return mapping_->get_spans_map().empty();
}

REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const SMatch& match) {
  return os << match.to_string();
}

}  // namespace library_interface
}  // namespace REmatch
