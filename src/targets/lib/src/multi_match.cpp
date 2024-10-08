#include <sys/types.h>
#include <REmatch/multi_match.hpp>
#include <cstdint>

#include "exceptions/variable_not_found_in_catalog_exception.hpp"

namespace REmatch {

inline namespace library_interface {

MultiMatch::MultiMatch(
    ExtendedMapping extended_mapping,
    std::shared_ptr<parsing::VariableCatalog> variable_catalog,
    std::shared_ptr<Document> document)
    : document_(document),
      variable_catalog_(variable_catalog),
      extended_mapping_(extended_mapping) {}

std::vector<Span> MultiMatch::spans(uint_fast32_t variable_id) {
  if (variable_id >= variable_catalog_->size()) {
    throw VariableNotFoundInCatalogException("");
  }

  if (mapping_cache_.has_value()) {
    return mapping_cache_.value()[int(variable_id)];
  }

  std::map<int, std::vector<Span>> mapping =
      extended_mapping_.construct_mapping();
  mapping_cache_ = mapping;
  return mapping[int(variable_id)];
}

std::vector<Span> MultiMatch::spans(const std::string& variable_name) {
  return spans(variable_catalog_->position(variable_name));
}

std::vector<std::string> MultiMatch::groups(uint_fast32_t variable_id) {
  if ((size_t)variable_id >= variable_catalog_->size()) {
    throw VariableNotFoundInCatalogException("");
  }

  std::map<int, std::vector<Span>> mapping;

  if (mapping_cache_.has_value()) {
    mapping = mapping_cache_.value();
  } else {
    mapping = extended_mapping_.construct_mapping();
    mapping_cache_ = mapping;
  }

  std::vector<std::string> strings;
  std::vector<Span>& spans = mapping[int(variable_id)];
  strings.reserve(spans.size());

  for (const auto& span : spans) {
    strings.push_back(document_->substr(span.first, span.second - span.first));
  }

  return strings;
}

std::vector<std::string> MultiMatch::groups(const std::string& variable_name) {
  return groups(variable_catalog_->position(variable_name));
}

MultiMatch MultiMatch::submatch(Span span) {
  ExtendedMapping submapping = extended_mapping_.get_submapping(span);
  return {submapping, variable_catalog_, document_};
}

bool MultiMatch::empty() {
  if (mapping_cache_.has_value()) {
    return mapping_cache_.value().empty();
  }

  std::map<int, std::vector<Span>> mapping =
      extended_mapping_.construct_mapping();
  mapping_cache_ = mapping;
  return mapping.empty();
}

bool MultiMatch::operator==(const MultiMatch& other) const {
  return this->extended_mapping_ == other.extended_mapping_ &&
         this->document_ == other.document_ &&
         this->variable_catalog_ == other.variable_catalog_;
}

std::ostream& operator<<(std::ostream& os, MultiMatch& match) {
  const auto num_variables = match.variable_catalog_->size();

  for (unsigned int i = 0; i < num_variables - 1; i++) {
    const auto variable_name = match.variable_catalog_->get_var(i);
    const auto spans = match.spans(variable_name);

    if (spans.empty()) {
      continue;
    }

    os << variable_name << " = |" << spans[0].first << "," << spans[0].second
       << ">";
    for (unsigned int j = 1; j < spans.size(); j++) {
      os << "; |" << spans[j].first << "," << spans[j].second << ">";
    }
    os << "\t";
  }

  const auto variable_name =
      match.variable_catalog_->get_var(num_variables - 1);
  const auto spans = match.spans(variable_name);

  if (spans.empty()) {
    return os;
  }

  os << variable_name << " = |" << spans[0].first << "," << spans[0].second
     << ">";
  for (unsigned int j = 1; j < spans.size(); j++) {
    os << "; |" << spans[j].first << "," << spans[j].second << ">";
  }

  return os;
}

}  // namespace library_interface

}  // namespace REmatch
