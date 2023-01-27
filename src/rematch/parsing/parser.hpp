#ifndef PARSER_HPP
#define PARSER_HPP

#include <string_view>

#undef private
#include "parsing/factories/variable_factory.hpp"

#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace rematch {

class Parser {


  public:

  private:
    std::shared_ptr<VariableFactory> vfact_ptr;
    std::unique_ptr<LogicalVA> logical_va;

  public:
    Parser(const std::string_view input);
    LogicalVA get_logical_va();

  private:
    void create_logical_va(std::string_view input);

};

} // end namespace rematch

#endif
