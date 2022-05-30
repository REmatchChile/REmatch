#ifndef PARSER_HPP
#define PARSER_HPP

#include "grammar.hpp"
#include "visitors.hpp"
#include <string>

namespace rematch {

bool doParse(const std::string &input, ast::altern &data);
std::unique_ptr<LogicalVA> regex2LVA(std::string regex);

} // end namespace rematch

#endif