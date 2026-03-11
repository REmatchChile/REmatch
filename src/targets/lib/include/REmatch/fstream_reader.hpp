#pragma once

#include <sstream>

#include "REmatch/constants.hpp"
#include "REmatch_export.hpp"

namespace REmatch {

// Abstract interface to read a document character by character.
class REMATCH_EXPORT Reader {
 public:

  // Reads the next character from the document. Copies the next character to `a`. Returns true if
  // successful, or false if it reached the end of the document.
  virtual bool read(char& a) = 0;

  virtual ~Reader() = default;
};

// Reader implementation to read a file.
class REMATCH_EXPORT FStreamReader : public Reader {
 public:

  // Creates a FStreamReader. Takes a basic_istream to read data from.
  explicit FStreamReader(std::basic_istream<char>& input);

  ~FStreamReader() override;

  // Reads the next character from the stream. Copies the next character to `a`. Returns true if
  // successful, or false if it reached the end of the document.
  bool read(char& a) override;

 private:
  std::basic_istream<char>& input;

  char* buffer;
  static const uint64_t READER_BUFFER_SIZE = 4096;
  uint64_t bytes_read = 0;
  uint64_t read_pos = 0;
};

}  // namespace REmatch
