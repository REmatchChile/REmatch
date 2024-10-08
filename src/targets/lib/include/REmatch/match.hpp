#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {
class Document;

inline namespace library_interface {

class Match {

 private:
  mediator::Mapping mapping_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;

 public:
  Match(mediator::Mapping mapping,
        std::shared_ptr<parsing::VariableCatalog> variable_catalog,
        std::shared_ptr<Document> document);

  int64_t start(const std::string& variable_name);
  int64_t start(uint_fast32_t variable_id);

  int64_t end(const std::string& variable_name);
  int64_t end(uint_fast32_t variable_id);

  Span span(const std::string& variable_name);
  Span span(uint_fast32_t variable_id);

  std::string group(const std::string& variable_name);
  std::string group(uint_fast32_t variable_id);

  std::map<std::string, Span> groupdict();

  std::vector<std::string> variables();

  bool empty();

  friend std::ostream& operator<<(std::ostream& os, Match& match);
};
}  // end namespace library_interface

}  // namespace REmatch
