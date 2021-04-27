#ifndef SETSTATE_HPP
#define SETSTATE_HPP

#include <vector>
#include <bitset>
#include <set>

#include "bitsetwrapper.hpp"

namespace rematch {

class ExtendedVA;
class State;

class SetState {
 public:
	std::set<State*> subset;
	BitsetWrapper bitstring;

	bool isFinal, isSuperFinal, isNonEmpty;

	friend std::ostream & operator<<(std::ostream &os, SetState const &ss);

	SetState(ExtendedVA &eVA, std::set<State*> l);
};

std::ostream & operator<<(std::ostream &os, SetState const &ss);

} // end namespace rematch

#endif
