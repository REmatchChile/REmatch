#ifndef LVASTATE_HPP
#define LVASTATE_HPP

#include <list>
#include <set>
#include <bitset>
#include <memory>

class LVAState;

class LVACapture{
public:
  LVAState* from;
  LVAState* next;
  std::bitset<32> code;
  bool flag;

  LVACapture(LVAState* from, std::bitset<32> coding, LVAState* next);

  void reset_states(LVAState* insiding, LVAState *to) {from = insiding; next=to;}

  bool operator==(const LVACapture &rhs) const;

  bool operator<(const LVACapture &rhs) const;
};

class LVAFilter {
public:
  LVAState* from;
  LVAState* next;
  unsigned int code;
  bool flag;

  LVAFilter(LVAState* from, unsigned int coding, LVAState* next): from(from), next(next), code(coding) {}

  void reset_states(LVAState *s) {next = s;}

  bool operator==(const LVAFilter &rhs) const {
    return (from == rhs.from) && (next == rhs.next) && (code == rhs.code);
  }

};

class LVAEpsilon {
public:
  LVAState* next;
  LVAEpsilon(LVAState* next): next(next) {}

  void reset_states(LVAState *s) {next = s;}
};

class LVAState {
  private:
    static unsigned int ID; // Static var to make sequential id's
  public:
    unsigned int id;               // id

    std::list<std::shared_ptr<LVAFilter>> f;    // Filter array
    std::list<std::shared_ptr<LVACapture>> c;  // Capture pointers array
    std::list<std::shared_ptr<LVAEpsilon>> e; // Epsilon transitions

    bool tempMark; // Booleans for graph algorithms
    char colorMark;

    long visitedBy;

    bool isFinal;
    bool isInit;
    bool isSuperFinal;

    std::list<std::shared_ptr<LVACapture>> incidentCaptures;
    std::list<std::shared_ptr<LVAFilter>> incidentFilters;

    LVAState();

    LVAState(const LVAState& s);

    void init();
    LVAState* nextLVAState(unsigned int code);
    void addCapture(std::bitset<32> code, LVAState* next);

    // Adds a filter transition according to a Filter code and a LVAState pointer
    // returns an std::pair containing a boolean seted to if the filter was already
    // in the transition table, and an std::shared_ptr to the added (or found)
    // filter.
    void addFilter(unsigned int code, LVAState* next);
    void addEpsilon(LVAState* next);
    void setFinal(bool b);
    void setInitial(bool b);

    bool operator==(const LVAState &rhs) const;
};


#endif


