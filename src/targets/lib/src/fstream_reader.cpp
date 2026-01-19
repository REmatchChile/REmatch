#include "REmatch/fstream_reader.hpp"

#include <iostream>

namespace REmatch {

FStreamReader::FStreamReader(std::basic_istream<char>& input) : input(input) {
  buffer = new char[READER_BUFFER_SIZE];
}

FStreamReader::~FStreamReader() {
  delete[] buffer;
}

bool FStreamReader::read(char& a) {
  if (read_pos < bytes_read) {
    a = buffer[read_pos++];
    return true;
  }

  input.read(buffer, sizeof(buffer));
  bytes_read = input.gcount();
  read_pos = 0;

  if (read_pos < bytes_read) {
    a = buffer[read_pos++];
    return true;
  }

  return false;
}
}  // namespace REmatch
