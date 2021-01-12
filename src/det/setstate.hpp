#ifndef SETSTATE_HPP
#define SETSTATE_HPP

#include <vector>
#include <bitset>
#include <set>

#include "bitsetwrapper.hpp"

class ExtendedVA;
class LVAState;

class SetState {
 public:
	std::set<LVAState*> subset;
	BitsetWrapper bitstring;

	bool isFinal, isSuperFinal, isNonEmpty;

	friend std::ostream & operator<<(std::ostream &os, SetState const &ss);

	SetState(ExtendedVA &eVA, std::set<LVAState*> l);
};

std::ostream & operator<<(std::ostream &os, SetState const &ss);

#endif
