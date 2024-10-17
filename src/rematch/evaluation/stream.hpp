#pragma once

#include <memory>
#include <string>

#include "library_interface/reader.hpp"

namespace rematch {

class Stream {
 public:
  explicit Stream(std::shared_ptr<Reader> reader) : reader(std::move(reader)) {}

  bool read(char& a) {
    if (start_char) {
      start_char = false;
      a = START_CHAR;
      return true;
    } else if (reader->read(a)) {
      return true;
    } else if (end_char) {
      a = END_CHAR;
      return true;
    }
    return false;
  }

 private:
  std::shared_ptr<Reader> reader;

  bool start_char = true;
  bool end_char = true;
};

}  // namespace rematch
