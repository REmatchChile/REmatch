#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "evaluation/start_end_chars.hpp"

using namespace rematch;

class Reader {
 public:
  virtual bool read(char& a) = 0;
  virtual ~Reader() = default;
};


class FileStreamReader : public Reader {
 public:
  explicit FileStreamReader(std::basic_istream<char>& input) : input(input) {
    buffer = new char[buffer_size];
    input.read(buffer, buffer_size);
    buffer_end = input.gcount();
  }

  ~FileStreamReader() override { delete[] buffer; }

  bool read(char& a) override {
    if (buffer_pos < buffer_end) {
      a = buffer[buffer_pos];
      buffer_pos++;
      return true;
    }

    if (!input.eof()) {
      input.read(buffer, buffer_size);
      buffer_end = input.gcount();
      a = buffer[0];
      buffer_pos = 1;
      return true;
    } else {
      return false;
    }
  }

  bool has_next_ = true;
  char* buffer;
  std::basic_istream<char>& input;
  int64_t buffer_size = 50;
  int64_t buffer_pos = 0;
  int64_t buffer_end = 0;
};
