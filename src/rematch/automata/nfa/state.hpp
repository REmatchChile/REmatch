#ifndef LVASTATE_HPP
#define LVASTATE_HPP

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
  LVAEpsilon(State* next): next(next) {}

  void reset_states(State *s) {next = s;}
};

class State {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    unsigned int id;               // id

    std::list<std::shared_ptr<LVAFilter>> filters;    // Filter array
    std::list<std::shared_ptr<LVACapture>> captures;  // Capture pointers array
    std::list<std::shared_ptr<LVAEpsilon>> epsilons; // Epsilon transitions

    bool tempMark; // Booleans for graph algorithms
    char colorMark;

    uint32_t visitedBy;

    bool isFinal;
    bool isInit;
    bool isSuperFinal;

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

    std::list<std::shared_ptr<LVACapture>> incidentCaptures;
    std::list<std::shared_ptr<LVAFilter>> incidentFilters;

    State();

    State(const State& s);

    void init();
    State* nextLVAState(unsigned int code);
    void addCapture(std::bitset<32> code, State* next);
    void addFilter(unsigned int code, State* next);
    void addEpsilon(State* next);
    void setFinal(bool b);
    void setInitial(bool b);

    uint32_t flags() const {return flags_;}
    void set_flags(uint32_t f) {flags_ = f;}

    bool operator==(const State &rhs) const;
};

} // end namespace rematch

#endif


