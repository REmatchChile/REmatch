#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "match.hpp"
#include "span.hpp"

#include "REmatch_export.hpp"

namespace REmatch {
class ExtendedMapping;

inline namespace library_interface {

class REMATCH_EXPORT MultiMatch {
 public:
  MultiMatch(std::unique_ptr<ExtendedMapping> extended_mapping,
             std::shared_ptr<parsing::VariableCatalog> variable_catalog,
             std::shared_ptr<Document> document);

  // Copy and move constructors
  MultiMatch(const MultiMatch& other);
  MultiMatch& operator=(const MultiMatch& other);

  MultiMatch(MultiMatch&& other) noexcept;
  MultiMatch& operator=(MultiMatch&& other) noexcept;

  ~MultiMatch();

  std::vector<Span> spans(uint_fast32_t variable_id) const;
  std::vector<Span> spans(const std::string& variable_name) const;

  std::vector<std::string> groups(uint_fast32_t variable_id) const;
  std::vector<std::string> groups(const std::string& variable_name) const;

  MultiMatch submatch(Span span) const;

  bool empty() const;

  std::vector<std::string> variables() const;

  bool operator==(const MultiMatch& other) const;

  std::string to_string() const;

  friend REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const MultiMatch& match);

 private:
  std::unique_ptr<ExtendedMapping> extended_mapping_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
  mutable std::unique_ptr<std::map<int, std::vector<Span>>> mapping_cache_;
};
}  // namespace library_interface
}  // namespace REmatch
