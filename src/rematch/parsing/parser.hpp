#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace rematch {

LogicalVA doParse(const std::string& input);
std::unique_ptr<LogicalVA> regex2LVA(std::string regex);

} // end namespace rematch

#endif
