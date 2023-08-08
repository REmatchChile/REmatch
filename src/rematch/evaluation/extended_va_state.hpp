#ifndef EXTENDED_VA_STATE_HPP
#define EXTENDED_VA_STATE_HPP

#include "extended_va_filter.hpp"
#include "extended_va_capture.hpp"
#include "../parsing/charclass.hpp"

namespace rematch {

class ExtendedVAState {
private:
  static unsigned int ID;
  bool is_initial_;
  bool is_accepting_;

public:
  unsigned int id;

  bool tempMark;

  std::vector<ExtendedVAFilter*> filters;
  std::vector<ExtendedVAFilter*> backward_filters;

  std::vector<ExtendedVACapture*> captures;
  std::vector<ExtendedVACapture*> backward_captures;

  ExtendedVAState() {}

  // void add_transition(CharClass charclass, ExtendedVAState* next);
  void add_capture(std::bitset<64> code, ExtendedVAState* next);
  void add_filter(rematch::parsing::CharClass charclass, ExtendedVAState* next);

  void set_accepting(bool is_accepting);
  void set_initial(bool is_initial);
  bool is_accepting();
  bool is_initial();
};

}

#endif
