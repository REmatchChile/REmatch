#ifndef INVALID_FLAG_TYPE_EXCEPTION_HPP
#define INVALID_FLAG_TYPE_EXCEPTION_HPP

#include <stdexcept>

namespace REMatch {

class InvalidFlagTypeException : public std::invalid_argument {
public:
    InvalidFlagTypeException(const std::string& message)
        : std::invalid_argument(message) {}
};

}

#endif
