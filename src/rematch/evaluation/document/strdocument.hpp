#ifndef EVALUATION__DOCUMENT__STRDOCUMENT_HPP
#define EVALUATION__DOCUMENT__STRDOCUMENT_HPP

#include "evaluation/document/document.hpp"

namespace rematch {

class StrDocument : public Document {

public:
  StrDocument() : data_(nullptr), size_(0) {}

  StrDocument(const std::string &str) : data_(str.data()), size_(str.size()) {}

  StrDocument(const char *str)
      : data_(str), size_(str == nullptr ? 0 : strlen(str)) {}

  ~StrDocument() override = default;

  std::string substring(int i, int j) const {
    return std::string(data_ + i, j - i);
  }

  struct char_iterator {

    char_iterator() = delete;

    char_iterator(const char *d, size_t s) : data_(d), size_(s), current_(d) {}

    const char &operator*() const { return *current_; }

    char_iterator &operator++() {
      current_++;
      return *this;
    }

    bool at_end() const { return current_ != data_ + size_; }

  private:
    const char *data_;
    size_t size_;
    const char *current_;
  }; // end char_iterator

  struct line_iterator {

    line_iterator() = delete;

    line_iterator(const char *d, size_t s)
        : data_(d), size_(s), current_(d), ended_(false) {
      next_line();
    }

    const std::string &operator*() const { return line_; }

    line_iterator &operator++() {
      next_line();
      return *this;
    }

    bool at_end() const { return !ended_; }

  private:
    void next_line() {
      if (current_ == data_ + size_) {
        ended_ = true;
        return;
      }
      auto result = std::find(current_, data_ + size_, '\n');
      size_t n = result - current_ + 1;
      if (result == data_ + size_) {
        n -= 1;
      }
      line_.assign(current_, n);
      if (result == data_ + size_) {
        current_ = result;
      } else {
        current_ = result + 1;
      }
    }

    const char *data_;
    size_t size_;
    const char *current_;
    std::string line_;
    bool ended_;
  }; // end line_iterator

  CharIterator begin() const {
    return CharIterator(std::move(char_iterator(data_, size_)));
  }
  EndCharIterator end() const { return EndCharIterator(); }

  LineIterator line_begin() const {
    return LineIterator(line_iterator(data_, size_));
  }
  EndLineIterator line_end() const { return EndLineIterator(); }

  size_t size() const { return size_; }

  const char &operator[](int index) { return data_[index]; }

private:
  const char *data_;
  size_t size_;
}; // end class StrDocument

} // end namespace rematch

#endif // EVALUATION__DOCUMENT__STRDOCUMENT_HPP