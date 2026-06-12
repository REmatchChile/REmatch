#pragma once

#include <map>

#include "evaluation/document.hpp"
#include "multi_match.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch::internal {

class MultiMatchStandard : public MultiMatch {
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

  std::unique_ptr<MultiMatch> clone() const override;

 private:
  std::unique_ptr<ExtendedMapping> extended_mapping_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
  mutable std::unique_ptr<std::map<int, std::vector<Span>>> mapping_cache_;
  uint32_t num_variables;
};

}  // namespace REmatch
