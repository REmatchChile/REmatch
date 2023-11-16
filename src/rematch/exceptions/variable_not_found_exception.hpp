#ifndef VARIABLE_NOT_FOUND_EXCEPTION_HPP
#define VARIABLE_NOT_FOUND_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace REMatch {

class VariableNotFoundException : public std::invalid_argument {
 public:
  VariableNotFoundException(const std::string& variable_name)
    : std::invalid_argument("Variable not found: " + variable_name) {}
};

}

#endif
