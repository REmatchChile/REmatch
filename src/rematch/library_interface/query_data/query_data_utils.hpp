#ifndef REGEX_DATA_UTILS_HPP
#define REGEX_DATA_UTILS_HPP

#include <string>
#include "../flags.hpp"
#include "regex_data.hpp"

namespace rematch {

RegexData get_regex_data(std::string_view pattern, Flags flags = Flags());
RegexData get_multi_regex_data(std::string_view pattern, Flags flags = Flags());

}  // namespace rematch

#endif
