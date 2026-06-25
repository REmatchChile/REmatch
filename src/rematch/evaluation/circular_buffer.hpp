#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include "REmatch/constants.hpp"
#include "REmatch/exceptions.hpp"
#include "REmatch/span.hpp"

namespace REmatch {

class CircularBuffer {
 public:
  explicit CircularBuffer(uint64_t size = DEFAULT_STREAM_BUFFER_SIZE) : buffer_size(size) {
    buffer = new char[size];
  }

  ~CircularBuffer() { delete[] buffer; }

  void insert(char& a) {
    buffer[file_pos % buffer_size] = a;
    file_pos++;
  }

  std::string substr(const Span span) const {
    if (static_cast<uint64_t>(span.second) > file_pos ||
        (file_pos >= buffer_size && static_cast<uint64_t>(span.first) < file_pos - buffer_size)) {
      throw EvaluationException(
          "It was not possible to obtain the group. Try using a bigger "
          "buffer.");
    }

    uint64_t substr_start = span.first % buffer_size;
    uint64_t substr_end = span.second % buffer_size;

    if (substr_start > substr_end) {
      return std::string(buffer + substr_start, buffer_size - substr_start) +
             std::string(buffer, substr_end);
    }
    size_t substr_size = span.second - span.first;
    return {buffer + substr_start, substr_size};
  }

  std::string_view get_segment(const Span& span) {
    uint64_t substr_start = span.first % buffer_size;
    uint64_t substr_end = span.second % buffer_size;

    if (substr_start > substr_end) {
      tmp_segment = "";
      tmp_segment.append(buffer + substr_start, buffer_size - substr_start);
      tmp_segment.append(buffer, substr_end);
      return {tmp_segment};
    }

    size_t size = span.second - span.first;
    uint64_t offset = span.first % buffer_size;
    return std::string_view{buffer + offset, size};
  }

  void increase_size() {
    char* new_buffer = new char[buffer_size * 2];
    std::memcpy(new_buffer, buffer, buffer_size);
    delete[] buffer;
    buffer = new_buffer;
    buffer_size *= 2;
  }

  char at(uint64_t pos) {
    pos = pos % buffer_size;
    return buffer[pos];
  }

  uint64_t buffer_size;

 private:
  std::string tmp_segment;
  uint64_t file_pos = 0;
  char* buffer;
};

}  // namespace REmatch
