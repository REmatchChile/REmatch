#include "document_utils.hpp"

namespace rematch {

std::string add_start_and_end_chars(std::string_view document_view) {
  std::string document;

  // add the start and end chars that match anchors
  document.reserve(document_view.size() + 2);
  document.push_back(START_CHAR);
  document.append(document_view);
  document.push_back(END_CHAR);
  return document;
}

}  // namespace rematch
