#ifndef SRC_REMATCH_AUTOMATA_NFA_STATE_HPP
#define SRC_REMATCH_AUTOMATA_NFA_STATE_HPP

#include <list>
#include <set>
#include <bitset>
#include <memory>

namespace rematch {

class LogicalVAState;

class LogicalVACapture{
public:
  LogicalVAState* from;
  LogicalVAState* next;
  std::bitset<64> code;
  bool flag;

  LogicalVACapture(LogicalVAState* from, std::bitset<64> coding, LogicalVAState* next);

  void reset_states(LogicalVAState* insiding, LogicalVAState *to) {from = insiding; next=to;}

  bool operator==(const LogicalVACapture &rhs) const;

  bool operator<(const LogicalVACapture &rhs) const;
};

class LogicalVAFilter {
public:
  LogicalVAState* from;
  LogicalVAState* next;
  unsigned int code;
  bool flag;

  LogicalVAFilter(LogicalVAState* from, unsigned int coding, LogicalVAState* next): from(from), next(next), code(coding) {}

  void reset_states(LogicalVAState *s) {next = s;}

  bool operator==(const LogicalVAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (code == rhs.code);
  }

};

class LogicalVAEpsilon {
public:
  LogicalVAState* next;
  LogicalVAState* from;
  LogicalVAEpsilon(LogicalVAState* from, LogicalVAState* next): next(next), from(from) {}

  void reset_states(LogicalVAState *s) {next = s;}
};

class LogicalVAState {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    unsigned int id;               // id

    std::list<std::shared_ptr<LogicalVAFilter>> filters;    // Filter array
    std::list<std::shared_ptr<LogicalVACapture>> captures;  // Capture pointers array
    std::list<std::shared_ptr<LogicalVAEpsilon>> epsilons;

    // Booleans for graph algorithms
    bool tempMark = false;
    char colorMark = 'w';
    unsigned int visitedBy = 0;

    unsigned int flags_;

    enum LogicalVAStateFlags {
      kDefaultLogicalVAState = 0,
      kFinalLogicalVAState = 1,
      kSuperFinalLogicalVAState = kFinalLogicalVAState << 1,
      kInitialLogicalVAState = kSuperFinalLogicalVAState << 1,
      kStartAnchorLogicalVAState = kInitialLogicalVAState << 1,
      kEndAnchorLogicalVAState = kStartAnchorLogicalVAState << 1,
      kCaptureLogicalVAState = kEndAnchorLogicalVAState << 1,
      kPreCaptureLogicalVAState = kCaptureLogicalVAState << 1
    };

    std::list<std::shared_ptr<LogicalVACapture>> backward_captures_;
    std::list<std::shared_ptr<LogicalVAFilter>> backward_filters_;
    std::list<std::shared_ptr<LogicalVAEpsilon>> backward_epsilons_;

    LogicalVAState();

    ~LogicalVAState();

    LogicalVAState(const LogicalVAState& s);

    void init();

    LogicalVAState* nextFilter(unsigned int code);
    LogicalVAState* nextCapture(std::bitset<64> code);

    void add_capture(std::bitset<64> code, LogicalVAState* next);
    void add_filter(unsigned int code, LogicalVAState* next);
    void add_epsilon(LogicalVAState* next);

    // Getters and setters
    bool initial() const { return flags_ & kInitialLogicalVAState; }
    void set_initial(bool b);

    bool accepting() const { return flags_ & kFinalLogicalVAState; }
    void set_accepting(bool b);

    bool super_final() const { return flags_ & kSuperFinalLogicalVAState; }

    unsigned int flags() const {return flags_;}
    void set_flags(unsigned int f) {flags_ = f;}

    bool operator==(const LogicalVAState &rhs) const;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_STATE_HPP
