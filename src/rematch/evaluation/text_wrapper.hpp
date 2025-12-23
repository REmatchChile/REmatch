#pragma once

#include <string>

class TextWrapper {
 public:
  virtual ~TextWrapper() = default;

  // Get the character at the given index, including the bounding characters
  virtual const char& operator[](std::string::size_type pos) const = 0;

  // Get the size of the original string, including the bounding characters
  virtual std::string::size_type size() const noexcept = 0;
};
