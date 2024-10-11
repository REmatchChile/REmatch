#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "match.hpp"

#include <REmatch/span.hpp>

namespace REmatch {
class ExtendedMapping;

inline namespace library_interface {

class MultiMatch {
 public:
  MultiMatch(std::unique_ptr<ExtendedMapping> extended_mapping,
             std::shared_ptr<parsing::VariableCatalog> variable_catalog,
             std::shared_ptr<Document> document);

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

  friend std::ostream& operator<<(std::ostream& os, MultiMatch& match);

 private:
  std::unique_ptr<ExtendedMapping> extended_mapping_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
  mutable std::optional<std::map<int, std::vector<Span>>> mapping_cache_ =
      std::nullopt;
};
}  // namespace library_interface
}  // namespace REmatch
