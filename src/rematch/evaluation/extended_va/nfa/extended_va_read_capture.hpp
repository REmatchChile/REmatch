#ifndef EXTENDED_VA_READ_CAPTURE_HPP
#define EXTENDED_VA_READ_CAPTURE_HPP

namespace REmatch {

class ExtendedVAState;

class ExtendedVAReadCapture {
 public:
  ExtendedVAState* next;
  ExtendedVAState* from;
  parsing::CharClass charclass;
  std::bitset<64> captures_set;

  ExtendedVAReadCapture(ExtendedVAState* from, parsing::CharClass charclass,
                         std::bitset<64> captures_set, ExtendedVAState* next)
      : next(next), from(from), charclass(charclass), captures_set(captures_set) {}

};
}  // namespace REmatch

#endif