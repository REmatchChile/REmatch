#include "s_multi_match.hpp"

namespace REmatch {

SMultiMatch::SMultiMatch(std::unique_ptr<ExtendedMapping> extended_mapping,
                         std::shared_ptr<parsing::VariableCatalog> variable_catalog,
                         std::shared_ptr<Stream> stream)
    : extended_mapping_(std::move(extended_mapping)),
      variable_catalog_(std::move(variable_catalog)),
      stream(std::move(stream)) {}

SMultiMatch::SMultiMatch(const SMultiMatch& other)
    : extended_mapping_(std::make_unique<ExtendedMapping>(*other.extended_mapping_)),
      variable_catalog_(other.variable_catalog_),
      stream(other.stream) {
  if (other.mapping_cache_ != nullptr) {
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(*other.mapping_cache_);
  }
}

SMultiMatch& SMultiMatch::operator=(const SMultiMatch& other) {
  if (this == &other) {
    return *this;
  }

  extended_mapping_ = std::make_unique<ExtendedMapping>(*other.extended_mapping_);
  variable_catalog_ = other.variable_catalog_;
  stream = other.stream;

  if (other.mapping_cache_ != nullptr) {
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(*other.mapping_cache_);
  } else {
    mapping_cache_ = nullptr;
  }

  return *this;
}

SMultiMatch::SMultiMatch(SMultiMatch&& other) noexcept
    : extended_mapping_(std::move(other.extended_mapping_)),
      variable_catalog_(std::move(other.variable_catalog_)),
      stream(std::move(other.stream)),
      mapping_cache_(std::move(other.mapping_cache_)) {}

SMultiMatch& SMultiMatch::operator=(SMultiMatch&& other) noexcept {
  extended_mapping_ = std::move(other.extended_mapping_);
  variable_catalog_ = std::move(other.variable_catalog_);
  stream = std::move(other.stream);
  mapping_cache_ = std::move(other.mapping_cache_);
  return *this;
}

SMultiMatch::~SMultiMatch() = default;

std::vector<Span> SMultiMatch::spans(uint_fast32_t variable_id) const {
  if (variable_id >= variable_catalog_->size()) {
    std::string var_name = variable_catalog_->get_var(variable_id);
    throw VariableNotFoundException(var_name);
  }

  if (mapping_cache_ == nullptr) {
    auto mapping = extended_mapping_->construct_mapping();
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(std::move(mapping));
  }

  return (*mapping_cache_)[int(variable_id)];
}

std::vector<Span> SMultiMatch::spans(const std::string& variable_name) const {
  return spans(variable_catalog_->position(variable_name));
}

std::vector<std::string> SMultiMatch::groups(uint_fast32_t variable_id) const {
  if ((size_t)variable_id >= variable_catalog_->size()) {
    std::string var_name = variable_catalog_->get_var(variable_id);
    throw VariableNotFoundException(var_name);
  }

  if (mapping_cache_ == nullptr) {
    auto mapping = extended_mapping_->construct_mapping();
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(std::move(mapping));
  }

  std::vector<std::string> strings;
  std::vector<Span>& spans = (*mapping_cache_)[int(variable_id)];
  strings.reserve(spans.size());

  for (const auto& span : spans) {
    strings.push_back(stream->substr(span));
  }

  return strings;
}

std::vector<std::string> SMultiMatch::groups(const std::string& variable_name) const {
  return groups(variable_catalog_->position(variable_name));
}

std::unique_ptr<MultiMatch> SMultiMatch::submatch(Span span) const {
  std::unique_ptr<ExtendedMapping> submapping = extended_mapping_->get_submapping(span);
  return std::make_unique<SMultiMatch>(std::move(submapping), variable_catalog_, stream);
}

std::vector<std::string> SMultiMatch::variables() const {
  return variable_catalog_->variables();
}

bool SMultiMatch::empty() const {
  if (mapping_cache_ == nullptr) {
    auto mapping = extended_mapping_->construct_mapping();
    mapping_cache_ = std::make_unique<std::map<int, std::vector<Span>>>(std::move(mapping));
  }

  return mapping_cache_->empty();
}

bool SMultiMatch::operator==(const SMultiMatch& other) const {
  return *this->extended_mapping_ == *other.extended_mapping_ && this->stream == other.stream &&
         this->variable_catalog_ == other.variable_catalog_;
}

std::string SMultiMatch::to_string() const {
  const auto num_variables = variable_catalog_->size();

  if (num_variables == 0) {
    return "{}";
  }

  std::stringstream ss;

  ss << "{";
  const auto variable_name = variable_catalog_->get_var(0);
  const auto spans_ = spans(0);
  if (spans_.empty()) {
    ss << variable_name << ": {}\t";
  } else {
    ss << variable_name << ": {|" << spans_[0].first << "," << spans_[0].second << ">";
    for (std::size_t j = 1; j < spans_.size(); j++) {
      ss << ", |" << spans_[j].first << "," << spans_[j].second << ">";
    }
    ss << "}";
  }

  for (std::size_t i = 1; i < num_variables; i++) {
    const auto variable_name = variable_catalog_->get_var(i);
    const auto spans_ = spans(variable_name);
    if (spans_.empty()) {
      ss << ", " << variable_name << ": {}\t";
    } else {
      ss << ", " << variable_name << ": {|" << spans_[0].first << "," << spans_[0].second << ">";
      for (std::size_t j = 1; j < spans_.size(); j++) {
        ss << ", |" << spans_[j].first << "," << spans_[j].second << ">";
      }
      ss << "}";
    }
  }
  ss << "}";

  return ss.str();
}

std::unique_ptr<MultiMatch> SMultiMatch::clone() const {
  return std::make_unique<SMultiMatch>(*this);
}

}  // namespace REmatch
