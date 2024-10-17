#pragma once

#include <exception>
#include <string>
#include <utility>

namespace REmatch {

class REmatchException : public std::exception {
 public:
  explicit REmatchException(std::string message)
      : message_(std::move(message)) {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  const std::string message_;
};
}  // namespace REmatch
