#pragma once

#include <sstream>

#include "REmatch/constants.hpp"

namespace REmatch {

class Reader {
 public:
  virtual bool read(char& a) = 0;
  virtual ~Reader() = default;
};

class FStreamReader : public Reader {
 public:
  explicit FStreamReader(std::basic_istream<char>& input);

  ~FStreamReader() override;

  bool read(char& a) override;

 private:
  std::basic_istream<char>& input;

  char* buffer;
  static const uint64_t READER_BUFFER_SIZE = 4096;
  uint64_t bytes_read = 0;
  uint64_t read_pos = 0;
};

}  // namespace REmatch
