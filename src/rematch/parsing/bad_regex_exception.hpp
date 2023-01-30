#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <stdexcept>
#include <string>

namespace rematch {
inline namespace parsing {

class BadRegexException : public std::logic_error {
public:
  BadRegexException(std::string error) : logic_error(error), error_(error) {}
  virtual const char *what() const noexcept { return error_.c_str(); }

protected:
  std::string error_;

};

}
}

#endif
