#ifndef FACTORIES__ABSFACTORY_HPP
#define FACTORIES__ABSFACTORY_HPP

#include <unordered_map>
#include <stdexcept>

// Abstract base class for the factories.
template <class T>
class AbsFactory {

 public:
  AbsFactory() : size_(0) {}

  // Element getter from code
  const T& get_element(uint32_t code) const {

    if(code >= size_)
      throw std::logic_error("Code not found in table.");

    return element_table_.at(code);
  }

  // Code getter from an element
  uint32_t get_code(T & elem) const {
    auto &stored_val = code_table_.at(elem);

    if(stored_val) return stored_val;

    throw std::logic_error("Element not found in table.");
  }

  // Add an element to the tables returning its code.
  uint32_t add_element(T elem) {
    auto &stored_val = code_table_[elem];

    if(stored_val) return stored_val;

    stored_val = size_;
    element_table_.push_back(elem);

    ++size_;

    return size_-1;
  }

  size_t size() const {return size_;}

  bool empty() const {return size_ == 0;}
  // bool is_member(T elem) {return }

  // Returns a vector with the stored elements in coding order.
  std::vector<T> scheme() const {return element_table_;}

 private:
  size_t size_;
  std::vector<T> element_table_;
  std::unordered_map<T, uint32_t> code_table_;

}; // end class AbsFactory

#endif // FACTORIES__ABSFACTORY_HPP