#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

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

class REMATCH_EXPORT Match {

 public:
  Match(std::unique_ptr<mediator::Mapping> mapping,
        std::shared_ptr<parsing::VariableCatalog> variable_catalog,
        std::shared_ptr<Document> document);

  // Copy and move constructors
  Match(const Match& other);
  Match& operator=(const Match& other);

  Match(Match&& other) noexcept;
  Match& operator=(Match&& other) noexcept;

  ~Match();

  int64_t start(const std::string& variable_name) const;
  int64_t start(uint_fast32_t variable_id) const;

  int64_t end(const std::string& variable_name) const;
  int64_t end(uint_fast32_t variable_id) const;

  Span span(const std::string& variable_name) const;
  Span span(uint_fast32_t variable_id) const;

  std::string group(const std::string& variable_name) const;
  std::string group(uint_fast32_t variable_id) const;

  std::map<std::string, Span> groupdict() const;

  std::vector<std::string> variables() const;

  bool empty() const;

  std::string to_string() const;

  friend REMATCH_EXPORT std::ostream& operator<<(std::ostream& os,
                                                 const Match& match);

 private:
  std::unique_ptr<mediator::Mapping> mapping_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
};
}  // end namespace library_interface

}  // namespace REmatch
