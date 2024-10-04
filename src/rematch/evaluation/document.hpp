#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <string>

namespace REmatch {

/**
 * Document is a wrapper around a string that logically simulates a document that
 * is between the bounding characters in order to be evaluated by REmatch's algorithm.
 * The original string must be alive during the lifetime of the Document instance in
 * order to be able to access it during the evaluation and match content inspection.
 *
 * The methods of this class are almost the same as std::string, only the ones
 * needed are implemented.
 *
 */
class Document {
 public:
  explicit Document(const std::string& str);

  // Get the character at the given index, including the bounding characters
  const char& operator[](std::string::size_type pos) const;

  // Get the size of the original string, including the bounding characters
  std::string::size_type size() const noexcept;

  // Find the first occurrence of the given character starting from the given
  // position, including the bounding characters
  std::string::size_type find(std::string::value_type ch,
                              std::string::size_type pos = 0) const;

  // Get the substring of the original string
  std::string substr(std::string::size_type pos = 0,
                     std::string::size_type count = std::string::npos) const;

 private:
  std::string str;
};

}  // namespace REmatch

#endif