#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <string_view>
#ifdef EMSCRIPTEN
// Emscripten doesn't support std::string_view
#include <string>
#endif

#include "evaluation/start_end_chars.hpp"

namespace rematch {

/**
 * Document is a wrapper around a string_view that logically simulates a document that
 * is between the bounding characters in order to be evaluated by REmatch's algorithm.
 * The original string must be alive during the lifetime of the Document instance in
 * order to be able to access it during the evaluation and match content inspection.
 *
 * The methods of this class are almost the same as std::string_view, only the ones
 * needed are implemented.
 *
 * As Emscripten doesn't support std::string_view, the string is stored in a std::string
 * so this optimization can't be used.
 */
class Document {
 public:
  explicit Document(std::string_view str);

  // Get the character at the given index, including the bounding characters
  const char& operator[](std::string_view::size_type pos) const;

  // Get the size of the original string, including the bounding characters
  std::string_view::size_type size() const noexcept;

  // Find the first occurrence of the given character starting from the given
  // position, including the bounding characters
  std::string_view::size_type find(std::string_view::value_type ch,
                                   std::string_view::size_type pos = 0) const;

  // Get the substring of the original string
  std::string substr(
      std::string_view::size_type pos = 0,
      std::string_view::size_type count = std::string_view::npos) const;

 private:
#ifdef EMSCRIPTEN
  // Emscripten doesn't support std::string_view
  std::string str;
#else
  std::string_view str;
#endif
};

}  // namespace rematch

#endif