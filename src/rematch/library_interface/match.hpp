#ifndef LIBRARY_INTERFACE__MATCH_HPP
#define LIBRARY_INTERFACE__MATCH_HPP

#include <cstdlib>
#include <string>
#include <string_view>
#include <utility>

#include <memory>
#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"
#include "parsing/variable_catalog.hpp"

namespace rematch {
class Document;
}

namespace REMatch {

inline namespace library_interface {

class Match {

 private:
  rematch::mediator::Mapping mapping_;
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<rematch::Document> document_;

 public:
  Match(rematch::mediator::Mapping mapping,
        std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
        std::shared_ptr<rematch::Document> document);

  int start(std::string variable_name);
  int start(int variable_id);
  int end(std::string variable_name);
  int end(int variable_id);
  Span span(std::string variable_name);
  Span span(int variable_id);
  std::string group(std::string variable_name);
  std::string group(int variable_id);
  std::map<std::string, Span> groupdict();
  std::vector<std::string> variables();
  bool empty();

  friend std::ostream& operator<<(std::ostream& os, Match& match);
};
}  // end namespace library_interface

}  // namespace REMatch

#endif
