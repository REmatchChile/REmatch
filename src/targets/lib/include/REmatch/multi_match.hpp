#pragma once

#include <cstdint>
#include <optional>
#include "match.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "output_enumeration/span.hpp"

namespace REmatch {
inline namespace library_interface {

class MultiMatch {
 public:
  MultiMatch(ExtendedMapping extended_mapping,
             std::shared_ptr<parsing::VariableCatalog> variable_catalog,
             std::shared_ptr<Document> document);

  std::vector<Span> spans(uint_fast32_t variable_id);
  std::vector<Span> spans(const std::string& variable_name);

  std::vector<std::string> groups(uint_fast32_t variable_id);
  std::vector<std::string> groups(const std::string& variable_name);

  MultiMatch submatch(Span span);

  bool empty();

  bool operator==(const MultiMatch& other) const;

  friend std::ostream& operator<<(std::ostream& os, MultiMatch& match);
 private:
  std::shared_ptr<Document> document_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  ExtendedMapping extended_mapping_;
  std::optional<std::map<int, std::vector<Span>>> mapping_cache_ = std::nullopt;
};
}  // namespace library_interface
}  // namespace REmatch
