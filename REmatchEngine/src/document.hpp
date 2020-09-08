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

  virtual void get(ref c) = 0;
  virtual void reset() = 0;
  virtual void terminate() = 0;
  virtual sz_t size() const = 0;
  virtual bool getline(std::string &str) = 0;
  virtual bool at_end() const = 0;
  virtual bool at_start() const = 0;

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

  virtual void get(Document::ref c) {c = *current_++;}

  virtual bool getline(std::string &str) {
      if(current_ == end()) return false;
      Document::const_ptr result = std::find(current_, data_ + size_, '\n');
      std::memcpy(&str[0], current_, result - current_);
      return true;
  }

  virtual void reset() {current_ = data_;}
  virtual void terminate() {current_ = data_ + size_;}

  virtual bool at_end() const {return current_ == end();}
  virtual bool at_start() const {return current_ == begin();}

  virtual std::string_view get_view(size_t pos, size_t endpos) {
      return std::string_view(data_+pos, endpos-pos);
  }

 private:
    Document::const_iterator data_;
    Document::sz_t size_;
    Document::const_iterator current_;

}; // end class StrDocument


class ChunkDocument : public Document {
  public:
    ChunkDocument(): data_(nullptr), size_(0), current_(nullptr) {}

    virtual void get(Document::ref c) {c = *current_++;}

    virtual bool getline(std::string &str) {
      if(current_ == end()) return false;
      Document::const_ptr result = std::find(current_, data_ + size_, '\n');
      std::memcpy(&str[0], current_, result - current_);
      return true;
    }

    Document::const_iterator begin() const {return data_;}
    Document::const_iterator end() const {return data_ + size_;}

    virtual Document::sz_t size() const {return size_;}

    virtual void reset() {current_ = data_;}
    virtual void terminate() {current_ = data_ + size_;}

    virtual bool ended() const {return current_ == end();}

    virtual std::string_view get_view(size_t pos, size_t endpos) {
        return std::string_view(data_+pos, endpos-pos);
    }

    // Sets the corresponding chunk for access through the interface
    void feed(const std::string& chunk) {
        data_ = chunk.data();
        size_ = chunk.size();
        current_ = data_;
    }


  private:
    Document::const_iterator data_;
    Document::sz_t size_;
    Document::const_iterator current_;

}; // end class ChunkDocument

#endif // DOCUMENT_HPP