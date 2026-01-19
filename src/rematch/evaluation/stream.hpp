#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "REmatch/fstream_reader.hpp"
#include "circular_buffer.hpp"
#include "evaluation/start_end_chars.hpp"

namespace REmatch {

class Stream {
 public:
  explicit Stream(Reader* reader, uint64_t buffer_size = DEFAULT_STREAM_BUFFER_SIZE)
      : stream_reader(reader),
        buffer(std::make_unique<CircularBuffer>(buffer_size)),
        buffer_size(buffer_size) {}

  // It reads from the stream. It returns true if reads correctly
  bool read(char& a);

  // Reads from buffer
  void read_back(char& a, uint64_t pos) const;

  std::string substr(const Span& span) const;

  std::string_view get_segment(const Span& span) const;

  uint64_t get_buffer_size() const;

 private:
  Reader* stream_reader;
  std::shared_ptr<CircularBuffer> buffer;

  uint64_t buffer_size;

  bool end_char = true;
};

}  // namespace REmatch
