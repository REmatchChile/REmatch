#pragma once

#include <string>

#include "text_wrapper.hpp"

namespace REmatch {

/**
 * Line is a wrapper around a string that logically simulates a document that
 * is between the bounding characters in order to be evaluated by REmatch's algorithm.
 * The original string must be alive during the lifetime of the Line instance in
 * order to be able to access it during the evaluation and match content inspection.
 *
 * This class utilizes a string_view instead of a const reference in order to
 * represent a line in the original document.
 *
 */
class Line : public TextWrapper {
 public:
  explicit Line(std::string_view str);

  // Get the character at the given index, including the bounding characters
  const char& operator[](std::string::size_type pos) const override;

  // Get the size of the original string, including the bounding characters
  std::string::size_type size() const noexcept override;

 private:
  std::string_view str;
};

}  // namespace REmatch
