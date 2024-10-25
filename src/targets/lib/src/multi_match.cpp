#include <REmatch/multi_match.hpp>

#include <REmatch/REmatch_export.hpp>
#include <REmatch/exceptions.hpp>

#include <cstdint>
#include <memory>

#include "evaluation/document.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {

inline namespace library_interface {

MultiMatch::MultiMatch(
    std::unique_ptr<ExtendedMapping> extended_mapping,
    std::shared_ptr<parsing::VariableCatalog> variable_catalog,
    std::shared_ptr<Document> document)
    : extended_mapping_(std::move(extended_mapping)),
      variable_catalog_(variable_catalog),
      document_(document) {}

MultiMatch::MultiMatch(const MultiMatch& other)
    : extended_mapping_(
          std::make_unique<ExtendedMapping>(*other.extended_mapping_)),
      variable_catalog_(other.variable_catalog_),
      document_(other.document_) {
  if (other.mapping_cache_ != nullptr) {
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(
        *other.mapping_cache_);
  }
}

MultiMatch& MultiMatch::operator=(const MultiMatch& other) {
  if (this == &other) {
    return *this;
  }

  extended_mapping_ =
      std::make_unique<ExtendedMapping>(*other.extended_mapping_);
  variable_catalog_ = other.variable_catalog_;
  document_ = other.document_;

  if (other.mapping_cache_ != nullptr) {
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(
        *other.mapping_cache_);
  } else {
    mapping_cache_ = nullptr;
  }

  return *this;
}

MultiMatch::MultiMatch(MultiMatch&& other) noexcept
    : extended_mapping_(std::move(other.extended_mapping_)),
      variable_catalog_(std::move(other.variable_catalog_)),
      document_(std::move(other.document_)),
      mapping_cache_(std::move(other.mapping_cache_)) {}

MultiMatch& MultiMatch::operator=(MultiMatch&& other) noexcept {
  extended_mapping_ = std::move(other.extended_mapping_);
  variable_catalog_ = std::move(other.variable_catalog_);
  document_ = std::move(other.document_);
  mapping_cache_ = std::move(other.mapping_cache_);
  return *this;
}

MultiMatch::~MultiMatch() = default;

std::vector<Span> MultiMatch::spans(uint_fast32_t variable_id) const {
  if (variable_id >= variable_catalog_->size()) {
    throw VariableNotFoundInCatalogException("");
  }

  if (mapping_cache_ == nullptr) {
    auto mapping = extended_mapping_->construct_mapping();
    mapping_cache_ =
        std::make_unique<std::map<int, std::vector<Span>>>(std::move(mapping));
  }

  return (*mapping_cache_)[int(variable_id)];
}

std::vector<Span> MultiMatch::spans(const std::string& variable_name) const {
  return spans(variable_catalog_->position(variable_name));
}

std::vector<std::string> MultiMatch::groups(uint_fast32_t variable_id) const {
  if ((size_t)variable_id >= variable_catalog_->size()) {
    throw VariableNotFoundInCatalogException("");
  }

  if (mapping_cache_ == nullptr) {
    auto mapping = extended_mapping_->construct_mapping();
    mapping_cache_ =
        std::make_unique<std::map<int, std::vector<Span>>>(std::move(mapping));
  }

  std::vector<std::string> strings;
  std::vector<Span>& spans = (*mapping_cache_)[int(variable_id)];
  strings.reserve(spans.size());

  for (const auto& span : spans) {
    strings.push_back(document_->substr(span.first, span.second - span.first));
  }

  return strings;
}

std::vector<std::string> MultiMatch::groups(
    const std::string& variable_name) const {
  return groups(variable_catalog_->position(variable_name));
}

MultiMatch MultiMatch::submatch(Span span) const {
  std::unique_ptr<ExtendedMapping> submapping =
      extended_mapping_->get_submapping(span);
  return {std::move(submapping), variable_catalog_, document_};
}

std::vector<std::string> MultiMatch::variables() const {
  return variable_catalog_->variables();
}

bool MultiMatch::empty() const {
  if (mapping_cache_ == nullptr) {
    auto mapping = extended_mapping_->construct_mapping();
    mapping_cache_ =
        std::make_unique<std::map<int, std::vector<Span>>>(std::move(mapping));
  }

  return mapping_cache_->empty();
}

bool MultiMatch::operator==(const MultiMatch& other) const {
  return *this->extended_mapping_ == *other.extended_mapping_ &&
         this->document_ == other.document_ &&
         this->variable_catalog_ == other.variable_catalog_;
}

std::string MultiMatch::to_string() const {
  std::stringstream ss;

  const auto num_variables = variable_catalog_->size();

  ss << "{";
  for (unsigned int i = 0; i < num_variables - 1; i++) {
    const auto variable_name = variable_catalog_->get_var(i);
    const auto spans_ = spans(variable_name);

    if (spans_.empty()) {
      ss << variable_name << ": {}\t";
      continue;
    }

    ss << variable_name << ": {|" << spans_[0].first << "," << spans_[0].second
       << ">";
    for (unsigned int j = 1; j < spans_.size(); j++) {
      ss << ", |" << spans_[j].first << "," << spans_[j].second << ">";
    }
    ss << "}, ";
  }

  const auto variable_name = variable_catalog_->get_var(num_variables - 1);
  const auto spans_ = spans(variable_name);

  ss << "{";
  if (spans_.empty()) {
    ss << variable_name << ": {}}";
    return ss.str();
  }

  ss << variable_name << ": {|" << spans_[0].first << "," << spans_[0].second
     << ">";
  for (unsigned int j = 1; j < spans_.size(); j++) {
    ss << ", |" << spans_.at(j).first << "," << spans_.at(j).second << ">";
  }
  ss << "}}";

  return ss.str();
}

REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const MultiMatch& match) {
  return os << match.to_string();
}

}  // namespace library_interface

}  // namespace REmatch
