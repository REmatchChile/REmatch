#ifndef EXTENDED_VA_STATE_HPP
#define EXTENDED_VA_STATE_HPP

#include "extended_va_capture.hpp"
#include "extended_va_filter.hpp"
#include "extended_va_read_capture.hpp"
#include "parsing/charclass.hpp"

namespace rematch {

class ExtendedVAState {
 private:
  static unsigned int ID;
  bool is_initial_;
  bool is_accepting_;

 public:
  unsigned int id;

  bool temp_mark;

  std::vector<ExtendedVAFilter*> filters;
  std::vector<ExtendedVAFilter*> backward_filters;

  std::vector<ExtendedVACapture*> captures;
  std::vector<ExtendedVACapture*> backward_captures;

  std::vector<ExtendedVAReadCapture*> read_captures;
  std::vector<ExtendedVAReadCapture*> backward_read_captures;

  ExtendedVAState() { id = ID++; }

  void add_capture(std::bitset<64> code, ExtendedVAState* next);
  void add_filter(parsing::CharClass charclass, ExtendedVAState* next);
  void add_read_capture(parsing::CharClass charclass, std::bitset<64> code,
                        ExtendedVAState* next);
  void create_read_captures_forward();

  void set_accepting(bool is_accepting);
  void set_initial(bool is_initial);
  bool is_accepting();
  bool is_initial();
};

}  // namespace rematch

#endif
