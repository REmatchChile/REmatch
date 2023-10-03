#include "library_interface/flags.hpp"
#include <cstdarg>
#include <stdexcept>

namespace REMatch {
inline namespace library_interface {

  Flags::Flags(int num, ...) {
  std::va_list arguments;
  va_start(arguments, num);
  for ( int x = 0; x < num; x++ )
    switch (va_arg(arguments, FlagTypes) ) {
      case LineByLine:
        line_by_line = false;
        break;
      case EarlyOutput:
        early_output = false;
        break;
      default:
        throw(std::invalid_argument(
              "Flags initialized with an invalid flag type."));
    }
  va_end (arguments);
  }
}
}
