#ifndef SRC_REMATCH_AUTOMATA_NFA_STATE_HPP
#define SRC_REMATCH_AUTOMATA_NFA_STATE_HPP

#include <bitset>
#include <list>
#include <memory>
#include <set>

#include "automata/nfa/lva.hpp"

namespace rematch {

struct LogicalVA::Capture {
  State *from;
  State *next;
  std::bitset<32> code;
  bool flag;

  Capture(State *from, std::bitset<32> coding, State *next);

  void reset_states(State *insiding, State *to) {
    from = insiding;
    next = to;
  }

  bool operator==(const LogicalVA::Capture &rhs) const;

  bool operator<(const LogicalVA::Capture &rhs) const;
};

struct LogicalVA::Filter {
  State *from;
  State *next;
  unsigned int code;
  bool flag;

  Filter(State *from, unsigned int coding, State *next)
      : from(from), next(next), code(coding) {}

  void reset_states(State *s) { next = s; }

  bool operator==(const LogicalVA::Filter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (code == rhs.code);
  }
};

struct LogicalVA::Epsilon {
  State *next;
  State *from;
  Epsilon(State *from, State *next) : next(next), from(from) {}

  void reset_states(State *s) { next = s; }
};

class LogicalVA::State {
 private:
  static unsigned int ID; // Static var to make sequential id's
 public:
  unsigned int id; // id

  std::list<std::shared_ptr<Filter>> filters;   // Filter array
  std::list<std::shared_ptr<Capture>> captures; // Capture pointers array
  std::list<std::shared_ptr<Epsilon>> epsilons;

  // Booleans for graph algorithms
  bool tempMark = false;
  char colorMark = 'w';
  uint32_t visitedBy = 0;

  uint32_t flags_;

  enum StateFlags {
    kDefaultState          = 0,
    kFinalState            = 1,
    kSuperFinalState       = kFinalState << 1,
    kInitialState          = kSuperFinalState << 1,
    kStartAnchorState      = kInitialState << 1,
    kEndAnchorState        = kStartAnchorState << 1,
    kCaptureState          = kEndAnchorState << 1,
    kPreCaptureState       = kCaptureState << 1
  };

  std::list<std::shared_ptr<Capture>> backward_captures_;
  std::list<std::shared_ptr<Filter>> backward_filters_;
  std::list<std::shared_ptr<Epsilon>> backward_epsilons_;

  State();

  ~State();

  State(const State &s);

  void init();

  State *nextFilter(unsigned int code);
  State *nextCapture(std::bitset<32> code);

  void add_capture(std::bitset<32> code, State *next);
  void add_filter(unsigned int code, State *next);
  void add_epsilon(State *next);

  // Getters and setters
  bool initial() const { return flags_ & kInitialState; }
  void set_initial(bool b);

  bool accepting() const { return flags_ & kFinalState; }
  void set_accepting(bool b);

  bool super_final() const { return flags_ & kSuperFinalState; }

  uint32_t flags() const { return flags_; }
  void set_flags(uint32_t f) { flags_ = f; }

  bool operator==(const State &rhs) const;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_STATE_HPP
