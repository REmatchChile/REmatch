
#pragma once

#include <memory>
#include <sstream>
#include <utility>
#include "REmatch/fstream_reader.hpp"
#include "REmatch/span.hpp"
#include "evaluation/circular_buffer.hpp"
#include "evaluation/document.hpp"
#include "evaluation/stream.hpp"

namespace REmatch {

class LineSplitterStr {
 public:
  explicit LineSplitterStr(std::shared_ptr<Document> document) : document(std::move(document)) {}

  // Return spans corresponding to each line in the document. The last span does not include the END_CHAR.
  std::unique_ptr<Span> get_line() {
    if (current_end >= document->size()) {
      return nullptr;
    }

    uint64_t new_end_pos = document->find('\n', current_end);

    uint64_t first;
    uint64_t second;

    if (new_end_pos != std::string::npos) {
      first = current_end;
      second = new_end_pos;

    } else {
      first = current_end;
      second = document->size() - 1;
    }

    current_end = second;
    current_end++;

    return std::make_unique<Span>(first, second);
  }

  const std::shared_ptr<Document> document;
  uint64_t current_end = 0;
};

class LineSplitterStream {
 public:
  explicit LineSplitterStream(std::shared_ptr<Stream> stream)
      : buffer_size(stream->get_buffer_size()), stream(std::move(stream)) {}

  // Return spans corresponding to each line in the document. The last span does not include the END_CHAR.
  std::unique_ptr<Span> get_line() {
    char a;
    while (stream->read(a)) {
      if (a == '\n') {
        auto i_pos = initial_pos;
        initial_pos = pos + 1;
        if (pos - i_pos > buffer_size) {
          throw EvaluationException("Line does not fit in the stream buffer.");
        }
        auto span = std::make_unique<Span>(i_pos, pos++);
        return span;
      }
      pos++;
    }

    // return nullptr if the last line was returned already or the last line is empty
    if (reached_end || initial_pos == pos) {
      return nullptr;
    }

    if (pos - 1 - initial_pos > buffer_size) {
      throw EvaluationException("Line does not fit in the stream buffer.");
    }

    reached_end = true;
    return std::make_unique<Span>(initial_pos, pos - 1);
  }

  uint64_t buffer_size;
  std::shared_ptr<Stream> stream;
  uint64_t pos = 0;
  uint64_t initial_pos = 0;
  bool reached_end = false;
};

}  // namespace REmatch
