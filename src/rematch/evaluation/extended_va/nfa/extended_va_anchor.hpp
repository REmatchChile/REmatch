#ifndef EXTENDED_VA_ANCHOR_HPP
#define EXTENDED_VA_ANCHOR_HPP

namespace rematch {

class ExtendedVAState;

class ExtendedVAAnchor {
private:
  bool is_start_;

public:
  ExtendedVAState* from;
  ExtendedVAState* next;

  ExtendedVAAnchor(
      ExtendedVAState* from, ExtendedVAState* next):
      from(from), next(next) {}

  bool operator==(const ExtendedVAAnchor &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (is_start_ == rhs.is_start_);
  }

  void set_start(bool start) {is_start_ = start;}
  bool is_start() {return is_start_;}
};

}

#endif
