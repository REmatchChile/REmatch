#ifndef EVALUATION__DOCUMENT__FILEDOCUMENT_HPP
#define EVALUATION__DOCUMENT__FILEDOCUMENT_HPP

#include "evaluation/document/document.hpp"

namespace rematch {

class FileDocument : public Document {
public:
  FileDocument(std::ifstream &is) : file_(&is) {}

  FileDocument(const std::string &s) : file_(new std::ifstream(s)) {}

  ~FileDocument() override = default;

  struct line_iterator {
    line_iterator() = delete;

    line_iterator(std::ifstream &f) : file_(&f) { next_line(); }

    const std::string &operator*() const { return line_; }

    line_iterator &operator++() {
      next_line();
      return *this;
    }

    bool at_end() const { return !!(*file_); }

  private:
    void next_line() {
      std::getline(*file_, line_);
      if (!file_->eof())
        line_ += '\n';
    }

    std::ifstream *file_;
    std::string line_;
  };

  struct char_iterator {

    char_iterator() = delete;

    char_iterator(std::ifstream &f) : file_(&f), buff_(1024, 0) {
      fill_buff();
      buff_it_ = buff_.begin();
    }

    const char &operator*() const { return *buff_it_; }

    char_iterator &operator++() {
      ++buff_it_;
      if (buff_it_ == buff_.end() && (*file_)) {
        fill_buff();
        buff_it_ = buff_.begin();
      }
      return *this;
    }

    bool at_end() const { return !(!(*file_) && buff_it_ == buff_.end()); }

  private:
    void fill_buff() {
      file_->read(buff_.data(), 1024);
      size_t rsz = file_->gcount();
      buff_.resize(rsz);
    }

    std::ifstream *file_;
    std::vector<char> buff_;
    std::vector<char>::iterator buff_it_;
  }; // end char_iterator

  CharIterator begin() const override {
    file_->clear();
    file_->seekg(0, std::ios::beg);
    return CharIterator(char_iterator(*file_));
  }
  EndCharIterator end() const override { return EndCharIterator(); }

  LineIterator line_begin() const override {
    file_->clear();
    file_->seekg(0, std::ios::beg);
    return LineIterator(line_iterator(*file_));
  }
  EndLineIterator line_end() const override { return EndLineIterator(); }

private:
  std::ifstream *file_;
  // std::string *line_;

}; // end class StrDocument

} // end namespace rematch

#endif // EVALUATION__DOCUMENT__FILEDOCUMENT_HPP