#ifndef MEMORY_LIMIT_EXCEEDED_EXCEPTION_HPP
#define MEMORY_LIMIT_EXCEEDED_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace REMatch {

class MemoryLimitExceededException : public std::runtime_error {
 public:
  MemoryLimitExceededException()
      : std::runtime_error("Memory limit exceeded during document evaluation.") {}
};

}  // namespace REMatch

#endif
