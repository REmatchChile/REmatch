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
      case MultiLine:
        multi_line = true;
        break;
      case DotNewLine:
        dot_new_line = true;
        break;
      case LineByLine:
        line_by_line = true;
        break;
      case EarlyOutput:
        early_output = true;
        break;
      case SaveAnchors:
        save_anchors = true;
        break;
      default:
        throw(std::invalid_argument(
              "Flags initialized with an invalid flag type."));
    }
  va_end (arguments);
  }
}
}
