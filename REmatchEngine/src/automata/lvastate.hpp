#ifndef LVASTATE_HPP
#define LVASTATE_HPP

#include <list>
#include <set>
#include <bitset>

class LVAState;

class LVACapture{
public:
  LVAState* from;
  LVAState* next;
  std::bitset<32> code;

  LVACapture(LVAState* from, std::bitset<32> coding, LVAState* next);

  bool operator==(const LVACapture &rhs) const;

  bool operator<(const LVACapture &rhs) const;
};

class LVAFilter {
public:
  LVAState* next;
  unsigned int code;

  LVAFilter(unsigned int coding, LVAState* next): next(next), code(coding) {}

};

class LVAEpsilon {
public:
  LVAState* next;
  LVAEpsilon(LVAState* next): next(next) {}
};

class LVAState {
  private:
    static unsigned int ID; // Static var to make sequential id's 
  public:
    unsigned int id;               // id 

    std::list<LVAFilter*> f;    // Filter array
    std::set<LVACapture> c;  // Capture pointers array
    std::list<LVAEpsilon*> e; // Epsilon transitions

    bool tempMark; // Booleans for graph algorithms
    char colorMark; 

    long visitedBy; 

    bool isFinal;
    bool isInit;
    bool isSuperFinal;

    std::set<LVACapture> incidentCaptures;
    std::list<LVAFilter*> incidentFilters;

    LVAState();
    void init();
    LVAState* nextLVAState(unsigned int code);
    void addCapture(std::bitset<32> code, LVAState* next);
    void addFilter(unsigned int code, LVAState* next);
    void addEpsilon(LVAState* next);
    void setFinal(bool b);
    void setInitial(bool b);

    bool operator==(const LVAState &rhs) const;
};


#endif


