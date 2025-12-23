#include "stream.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {

bool Stream::read(char& a) {
#ifdef TRACY_ENABLE
  ZoneScopedNC("Stream::read", 0x458588);
#endif
  if (stream_reader->read(a)) {
    buffer->insert(a);
    return true;
  }
  if (end_char) {
    a = END_CHAR;
    buffer->insert(a);
    end_char = false;
    return true;
  }

  return false;
}

void Stream::read_back(char& a, uint64_t pos) const {
  a = buffer->at(pos);
}

std::string Stream::substr(Span& span) const {
  return buffer->substr(span);
}

std::string_view Stream::get_segment(Span& span) const {
  return buffer->get_segment(span);
}

uint64_t Stream::get_buffer_size() const {
  return buffer_size;
}

}  // namespace REmatch
