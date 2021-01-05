#ifndef STRUCTS__PRE_VECTOR_HPP
#define STRUCTS__PRE_VECTOR_HPP

#include <array>
#include <vector>

namespace rematch {

const size_t INIT_SIZE = 10;

template<typename T>
class prevector {
 public:
  using value_type      = T;
  using reference       = T&;
  using const_reference = const T&;
  using pointer         = T *;
  using const_pointer   = const T *;
  using size_type       = unsigned int;
  using iterator        = T *;


  prevector()
    : sz_(0) {}

  size_type size() { return sz_;}

  reference operator[] (size_type idx) {
    // if (idx < INIT_SIZE)
      return arr_[idx];
    // else
      // return aux_[idx-INIT_SIZE];
  }

  void push_back(const_reference val) {
    // if(sz_ < INIT_SIZE) {
      arr_[sz_] = val;
    // } else {
      // aux_.push_back(val);
    // }
    sz_++;
  }

  template<typename... Args>
  void emplace_back(Args && ...args) {
    // if(sz_ < INIT_SIZE) {
      new (&arr_[sz_]) T(std::forward<Args>(args)...);
    // } else {
      // aux_.emplace_back(std::forward<Args>(args)...);
    // }
    sz_++;
  }

 private:
  size_type sz_;
  T arr_[INIT_SIZE];
  // std::vector<T> aux_;
}; // end class prevector



} // end namespace rematch

#endif // STRUCTS__PRE_VECTOR_HPP

