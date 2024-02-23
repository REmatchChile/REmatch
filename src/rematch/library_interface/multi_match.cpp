#include "multi_match.hpp"
#include "exceptions/variable_not_found_in_catalog_exception.hpp"

namespace REMatch {
using namespace rematch;

MultiMatch::MultiMatch(
    ExtendedMapping extended_mapping,
    std::shared_ptr<parsing::VariableCatalog> variable_catalog,
    std::shared_ptr<Document> document)
    : document_(document),
      variable_catalog_(variable_catalog),
      extended_mapping_(extended_mapping) {}

std::vector<Span> MultiMatch::spans(int variable_id) {
  if ((size_t)variable_id >= variable_catalog_->size()) {
    throw REMatch::VariableNotFoundInCatalogException("");
  }

  if (mapping_cache_.has_value()) {
    return mapping_cache_.value()[variable_id];
  }

  std::map<int, std::vector<Span>> mapping =
      extended_mapping_.construct_mapping();
  mapping_cache_ = mapping;
  return mapping[variable_id];
}

std::vector<Span> MultiMatch::spans(std::string variable_name) {
  return spans(variable_catalog_->position(variable_name));
}

std::vector<std::string> MultiMatch::groups(int variable_id) {
  if ((size_t)variable_id >= variable_catalog_->size()) {
    throw REMatch::VariableNotFoundInCatalogException("");
  }

  std::map<int, std::vector<Span>> mapping;

  if (mapping_cache_.has_value()) {
    mapping = mapping_cache_.value();
  } else {
    mapping = extended_mapping_.construct_mapping();
    mapping_cache_ = mapping;
  }

  std::vector<std::string> strings;
  std::vector<Span>& spans = mapping[variable_id];
  strings.reserve(spans.size());

  for (const auto& span : spans) {
    strings.push_back(document_->substr(span.first, span.second - span.first));
  }

  return strings;
}

std::vector<std::string> MultiMatch::groups(std::string variable_name) {
  return groups(variable_catalog_->position(variable_name));
}

MultiMatch MultiMatch::submatch(Span span) {
  ExtendedMapping submapping = extended_mapping_.get_submapping(span);
  return MultiMatch(submapping, variable_catalog_, document_);
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

}  // namespace REMatch
