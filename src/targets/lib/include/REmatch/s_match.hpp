#pragma once

#include <memory>
#include <string>
#include <utility>

#include "span.hpp"

#include "REmatch_export.hpp"

namespace REmatch {

namespace mediator {
class Mapping;
}

inline namespace parsing {
class VariableCatalog;
}
class Stream;

inline namespace library_interface {

class REMATCH_EXPORT SMatch {
 public:
  SMatch(std::unique_ptr<mediator::Mapping> mapping,
         std::shared_ptr<VariableCatalog> variable_catalog,
         std::shared_ptr<Stream> stream);

  ~SMatch();

  Span span(const std::string& variable_name) const;

  Span span(uint_fast32_t variable_id) const;

  std::string group(const std::string& variable_name) const;

  std::string group(uint_fast32_t variable_id) const;

  std::string to_string() const;

  friend REMATCH_EXPORT std::ostream& operator<<(std::ostream& os,
                                                 const SMatch& match);

 private:
  std::unique_ptr<mediator::Mapping> mapping_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::shared_ptr<Stream> stream;
};

}  // namespace library_interface
}  // namespace REmatch
