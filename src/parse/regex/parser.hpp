#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "visitors.hpp"
#include "grammar.hpp"

namespace rematch {

bool doParse(const std::string& input, ast::altern &data);
std::unique_ptr<LogicalVA> regex2LVA(std::string regex);

} // end namespace rematch

#endif