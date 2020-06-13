#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <sstream>
#include <unordered_map>

#include "detstate.hpp"
#include "structures.hpp"
#include "captures.hpp"
#include "det/setstate.hpp"
#include "bitsetwrapper.hpp"

/****** CONSTRUCTORS ********/

DetState :: DetState()
  : visited(0),
    currentL(new NodeList()),
    copiedList(new NodeList()),
#ifdef NOPT_CROSSPROD
    oldL(new NodeList()),
#endif
    isFinal(false)

     {

  id = ID++;
  label = "{0}";
  for (size_t i = 0; i < 128; i++)
  {
    singleFilters[i] = nullptr;
  }

}

DetState :: DetState(SetState* ss):
  visited(0),
  currentL(new NodeList()),
  copiedList(new NodeList()),
#ifdef NOPT_CROSSPROD
  oldL(new NodeList()),
#endif
  ss(ss)
  {
  id = ID++;
  isFinal = ss->isFinal;
  isSuperFinal = ss->isSuperFinal;
  mark = false;
  std::stringstream s;
  s << *(ss);
  label = s.str();
  for (size_t i = 0; i < 128; i++)
  {
    singleFilters[i] = nullptr;
  }
}

void DetState :: setFinal(bool b) {
  isFinal = b;
}

void DetState :: setSubset(SetState* newss) {
  ss = newss;
  isFinal = ss->isFinal;
  isSuperFinal = ss->isSuperFinal;
}

void DetState :: addCapture(std::bitset<32> S, DetState* next_state) {
  Capture* cap = new Capture(S, next_state);
  c.push_back(cap);
}

void DetState :: addFilter(BitsetWrapper charBitset, DetState* nextState) {
  assert(!filters.count(charBitset));

  filters[charBitset] = nextState;
}


void DetState :: addFilter(char a, DetState *nextState) {

  singleFilters[(int)a] = nextState;
}

unsigned int DetState :: ID = 0;


DetState* DetState :: nextState(BitsetWrapper charBitset) {
  auto it = filters.find(charBitset);

  if(it != filters.end()) return it->second;

  return nullptr;
}

DetState* DetState :: nextState(char a) {
  if((int)a > 0 && (int)a < 128) {
    return singleFilters[int(a)];
  }

  return nullptr;
}

std::ostream & operator<<(std::ostream &os, DetState const &q) {
  return os << *(q.ss);
}

