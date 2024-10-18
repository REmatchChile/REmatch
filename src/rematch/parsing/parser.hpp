#ifndef PARSER_HPP
#define PARSER_HPP

#include <string_view>

#include "parsing/variable_catalog.hpp"

#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace REmatch {
inline namespace parsing {

class Parser {
 private:
  std::shared_ptr<VariableCatalog> vfact_ptr;
  std::unique_ptr<LogicalVA> logical_va;

 public:
  Parser(const std::string& input, bool allow_multi_spanners = false);

  LogicalVA& get_logical_va();
  std::shared_ptr<VariableCatalog> get_variable_catalog();

 private:
  void create_logical_va(const std::string& input,
                         bool allow_multi_spanners = false);
};

}  // namespace parsing
}  // namespace REmatch

#endif
