#include <stdexcept>

namespace REMatch {

class InvalidFlagTypeException : public std::invalid_argument {
public:
    InvalidFlagTypeException(const std::string& message)
        : std::invalid_argument(message) {}
};

}
