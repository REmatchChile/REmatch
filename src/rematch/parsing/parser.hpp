#ifndef PARSER_HPP
#define PARSER_HPP

#include <string_view>

#undef private
#include "parsing/variable_catalog.hpp"

#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "tracy/Tracy.hpp"

namespace rematch {
inline namespace parsing {

class Parser {
 private:
  std::shared_ptr<VariableCatalog> vfact_ptr;
  std::unique_ptr<LogicalVA> logical_va;

 public:
  Parser(const std::string_view input, bool allow_multi_spanners = false);
  ~Parser();

  LogicalVA& get_logical_va();
  std::shared_ptr<VariableCatalog> get_variable_catalog();

 private:
  void create_logical_va(std::string_view input,
                         bool allow_multi_spanners = false);
};

}  // namespace parsing
}  // namespace rematch

#endif
