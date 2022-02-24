#ifndef SRC_REMATCH_AUTOMATA_NFA_STATE_HPP
#define SRC_REMATCH_AUTOMATA_NFA_STATE_HPP

#include <list>
#include <set>
#include <bitset>
#include <memory>

namespace rematch {

class State;

class LVACapture{
public:
  State* from;
  State* next;
  std::bitset<32> code;
  bool flag;

  LVACapture(State* from, std::bitset<32> coding, State* next);

  void reset_states(State* insiding, State *to) {from = insiding; next=to;}

  bool operator==(const LVACapture &rhs) const;

  bool operator<(const LVACapture &rhs) const;
};

class LVAFilter {
public:
  State* from;
  State* next;
  unsigned int code;
  bool flag;

  LVAFilter(State* from, unsigned int coding, State* next): from(from), next(next), code(coding) {}

  void reset_states(State *s) {next = s;}

  bool operator==(const LVAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (code == rhs.code);
  }

};

class LVAEpsilon {
public:
  State* next;
  State* from;
  LVAEpsilon(State* from, State* next): next(next), from(from) {}

  void reset_states(State *s) {next = s;}
};

class State {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    unsigned int id;               // id

    std::list<std::shared_ptr<LVAFilter>> filters;    // Filter array
    std::list<std::shared_ptr<LVACapture>> captures;  // Capture pointers array
    std::list<std::shared_ptr<LVAEpsilon>> epsilons;

    // Booleans for graph algorithms
    bool tempMark = false;
    char colorMark = 'w';
    uint32_t visitedBy = 0;

    uint32_t flags_;

    enum StateFlags {
      kDefaultState = 0,
      kFinalState = 1,
      kSuperFinalState = kFinalState << 1,
      kInitialState = kSuperFinalState << 1,
      kStartAnchorState = kInitialState << 1,
      kEndAnchorState = kStartAnchorState << 1,
      kCaptureState = kEndAnchorState << 1,
      kPreCaptureState = kCaptureState << 1
    };

    std::list<std::shared_ptr<LVACapture>> backward_captures_;
    std::list<std::shared_ptr<LVAFilter>> backward_filters_;
    std::list<std::shared_ptr<LVAEpsilon>> backward_epsilons_;

    State();

    ~State();

    State(const State& s);

    void init();

    State* nextFilter(unsigned int code);
    State* nextCapture(std::bitset<32> code);

    void add_capture(std::bitset<32> code, State* next);
    void add_filter(unsigned int code, State* next);
    void add_epsilon(State* next);

    // Getters and setters
    bool initial() const { return flags_ & kInitialState; }
    void set_initial(bool b);

    bool accepting() const { return flags_ & kFinalState; }
    void set_accepting(bool b);

    bool super_final() const { return flags_ & kSuperFinalState; }

    uint32_t flags() const {return flags_;}
    void set_flags(uint32_t f) {flags_ = f;}

    bool operator==(const State &rhs) const;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_STATE_HPP


