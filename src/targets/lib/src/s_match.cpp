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

REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const SMatch& match) {
  return os << match.to_string();
}

}  // namespace library_interface
}  // namespace REmatch
