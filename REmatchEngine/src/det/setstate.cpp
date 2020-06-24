#include <vector>
#include <bitset>
#include <set>

#include "setstate.hpp"
#include "automata/lvastate.hpp"
#include "automata/eva.hpp"

SetState :: SetState(ExtendedVA &eVA, std::set<LVAState*> l):
	subset(l), bitstring(eVA.size()) {
	/* Construct the bitstring inmediately */
	isFinal = false;
	isSuperFinal = false;
	isNonEmpty = false;
	for(auto &state: subset) {
		bitstring.set(state->id,true);

		// Check if the subset is a final subset
		if(state->isFinal) isFinal = true;
		if(state->isSuperFinal) isSuperFinal = true;
	}

	if(bitstring.count()) isNonEmpty = true;
}


std::ostream & operator<<(std::ostream &os, SetState const &ss) {
  if(ss.subset.empty()) return os << "{}";
  os << "{";
  for(auto &state: ss.subset) {
  	if(state == *ss.subset.begin()) {
  		os << state->id;
  	}
  	else {
  		os << "," << state->id;
  	}
  }
  os << "}";
  return os;
}

