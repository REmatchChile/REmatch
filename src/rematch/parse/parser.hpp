#ifndef PARSER_HPP
#define PARSER_HPP

#include "grammar.hpp"
#include "visitors.hpp"
#include <string>

namespace rematch {

bool doParse(std::string_view input, ast::altern &data);
std::unique_ptr<LogicalVA> regex2LVA(std::string_view regex);

} // end namespace rematch

#endif