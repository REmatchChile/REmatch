#ifndef QUERY_DATA_UTILS_HPP
#define QUERY_DATA_UTILS_HPP

#include <string>
#include "../flags.hpp"
#include "query_data.hpp"

namespace rematch {

QueryData get_query_data(std::string_view pattern, Flags flags = Flags());
QueryData get_multi_query_data(std::string_view pattern, Flags flags = Flags());

}  // namespace rematch

#endif
