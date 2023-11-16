#ifndef COMPLEX_QUERY_EXCEPTION_HPP
#define COMPLEX_QUERY_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace REMatch {

class ComplexQueryException : public std::runtime_error {
 public:
  ComplexQueryException()
      : std::runtime_error("Query is too complex to evaluate.") {}
};
}  // namespace REMatch

#endif
