#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "match.hpp"
#include "span.hpp"

#include "REmatch_export.hpp"

namespace REmatch {
class Document;

namespace mediator {
class Mapping;
}

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {

class REMATCH_EXPORT MatchStandard : public Match {

 public:
  MatchStandard(std::unique_ptr<mediator::Mapping> mapping,
        std::shared_ptr<parsing::VariableCatalog> variable_catalog,
        std::shared_ptr<Document> document);

  // Copy and move constructors
  MatchStandard(const MatchStandard& other);
  MatchStandard& operator=(const MatchStandard& other);

  MatchStandard(MatchStandard&& other) noexcept;
  MatchStandard& operator=(MatchStandard&& other) noexcept;

  ~MatchStandard() override;

  int64_t start(const std::string& variable_name) const override;
  int64_t start(uint_fast32_t variable_id) const override;

  int64_t end(const std::string& variable_name) const override;
  int64_t end(uint_fast32_t variable_id) const override;

  Span span(const std::string& variable_name) const override;
  Span span(uint_fast32_t variable_id) const override;

  std::string group(const std::string& variable_name) const override;
  std::string group(uint_fast32_t variable_id) const override;

  std::map<std::string, Span> groupdict() const override;

  std::vector<std::string> variables() const override;

  bool empty() const override;

  std::string to_string() const override;

  friend REMATCH_EXPORT std::ostream& operator<<(std::ostream& os, const MatchStandard& match);

 private:
  std::unique_ptr<mediator::Mapping> mapping_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
};
}  // end namespace library_interface

}  // namespace REmatch
