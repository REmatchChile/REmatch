#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "multi_match.hpp"
#include "span.hpp"

#include "REmatch_export.hpp"

namespace REmatch {
class ExtendedMapping;
class Document;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {

class REMATCH_EXPORT MultiMatchStandard : public MultiMatch {
 public:
  MultiMatchStandard(std::unique_ptr<ExtendedMapping> extended_mapping,
                     std::shared_ptr<parsing::VariableCatalog> variable_catalog,
                     std::shared_ptr<Document> document);

  // Copy and move constructors
  MultiMatchStandard(const MultiMatchStandard& other);
  MultiMatchStandard& operator=(const MultiMatchStandard& other);

  MultiMatchStandard(MultiMatchStandard&& other) noexcept;
  MultiMatchStandard& operator=(MultiMatchStandard&& other) noexcept;

  ~MultiMatchStandard() override;

  std::vector<Span> spans(uint_fast32_t variable_id) const override;
  std::vector<Span> spans(const std::string& variable_name) const override;

  std::vector<std::string> groups(uint_fast32_t variable_id) const override;
  std::vector<std::string> groups(const std::string& variable_name) const override;

  std::unique_ptr<MultiMatch> submatch(Span span) const override;

  bool empty() const override;

  std::vector<std::string> variables() const override;

  bool operator==(const MultiMatchStandard& other) const;

  std::string to_string() const override;

  friend REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const MultiMatchStandard& match);

 private:
  std::unique_ptr<ExtendedMapping> extended_mapping_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
  mutable std::unique_ptr<std::map<int, std::vector<Span>>> mapping_cache_;
};
}  // namespace library_interface
}  // namespace REmatch
