#include "flags.hpp"

namespace REMatch {
inline namespace library_interface {

Flags::Flags(int num, ...) {
  std::va_list arguments;
  va_start(arguments, num);
  for (int x = 0; x < num; x++)
    switch (va_arg(arguments, FlagTypes)) {
      case LineByLine:
        line_by_line = true;
        break;
      case EarlyOutput:
        early_output = true;
        break;
      default:
        throw InvalidFlagTypeException("Flags initialized with an invalid flag type.");
    }
  va_end(arguments);
}

}  // namespace library_interface
}  // namespace REMatch
