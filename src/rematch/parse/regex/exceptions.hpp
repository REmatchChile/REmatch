#ifndef PARSER__EXCEPTIONS
#define PARSER__EXCEPTIONS

#include <exception>

namespace rematch {
namespace parsing {

class BadRegex : public std::logic_error {
 public:
  BadRegex(std::string error) : logic_error(error), error_(error) {}
  virtual const char * what() const noexcept {return error_.c_str();}

 protected:
  std::string error_;

}; // end class bad_regex

} // end namespace parsing
} // end namespace rematch

#endif // PARSER_EXCEPTIONS