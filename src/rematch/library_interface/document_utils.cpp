#include <fstream>
#include <iostream>
#include <string>

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

std::string read_file_and_add_start_and_end_chars(std::ifstream& file) {
  file.seekg(0, std::ios::end);
  std::streampos fileSize = file.tellg();

  std::string result;
  result.resize(static_cast<size_t>(fileSize) + 2);

  file.seekg(0, std::ios::beg);

  file.read(&result[1], fileSize);

  // add the start and end chars
  result[0] = START_CHAR;
  result[static_cast<size_t>(fileSize) + 1] = END_CHAR;

  file.close();

  return result;
}

std::string read_file(std::ifstream& file) {
  file.seekg(0, std::ios::end);
  std::streampos fileSize = file.tellg();

  std::string result;
  result.resize(static_cast<size_t>(fileSize));

  file.seekg(0, std::ios::beg);

  file.read(&result[0], fileSize);
  file.close();

  return result;
}

std::string_view get_document_as_string_view(std::string&& document) {
  size_t end_index = document.size() - 1;
  // it represents the document without including the START and END
  std::string_view document_view(document.data() + 1, end_index - 1);
  return document_view;
}

}  // namespace rematch
