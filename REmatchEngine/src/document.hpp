#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <string>
#include <istream>
#include <string_view>

class Document {
 public:
  using value_t = char;
  using ptr = char*;
  using const_ptr = const char*;
  using ref = char&;
  using const_ref = const char&;
  using const_iterator = const char*;
  using iterator = Document::const_iterator;
  using sz_t = size_t;

  virtual void get(value_t &c) = 0;
  virtual void reset() = 0;
  virtual sz_t size() const = 0;
  virtual bool getline(std::string &str) = 0;

  virtual std::string_view get_view(size_t pos, size_t endpos) = 0;
}; // end class Document


class StrDocument : public Document {
 public:
  StrDocument()
      : data_(nullptr), size_(0), current_(0) {}

  StrDocument(const std::string &str)
      : data_(str.data()), size_(str.size()), current_(str.data()) {}

  StrDocument(const char* str)
      : data_(str), size_(str == nullptr ? 0 : strlen(str)), current_(str) {}

  Document::const_iterator begin() const {return data_;}
  Document::const_iterator end() const {return data_ + size_;}

  virtual Document::sz_t size() const {return size_;}

  virtual void get(char &c) {c=*current_++;}

  virtual bool getline(std::string &str) {
      if(current_ == end()) return false;
      Document::const_ptr result = std::find(current_, data_ + size_, '\n');
      std::memcpy(&str[0], current_, result - current_);
      return true;
  }

  virtual void reset() {current_ = data_;}

  virtual std::string_view get_view(size_t pos, size_t endpos) {
      return std::string_view(data_+pos, endpos-pos);
  }

 private:
    Document::const_iterator data_;
    Document::sz_t size_;
    Document::const_iterator current_;

}; // end class StrDocument


class FileDocument : public Document {
 public:
  FileDocument()
      : data_(nullptr), size_(0) {}

  FileDocument(std::istream &is)
      : data_(&is), size_(0) {}

  virtual Document::sz_t size() const {return size_;}

  virtual void get(char &c) {data_->get(c);}

  virtual bool getline(std::string &str) {
      return !(std::getline(*data_ ,str).fail());
  }

  virtual void reset() {data_->seekg(0);}

  virtual std::string_view get_view(size_t pos, size_t endpos) {
      return std::string_view();
  }


 private:
    std::istream *data_;
    Document::sz_t size_;

}; // end class StrDocument

#endif // DOCUMENT_HPP