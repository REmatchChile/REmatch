#ifndef DETMANAGER_HPP
#define DETMANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>

#include "bitsetwrapper.hpp"
#include "det/setstate.hpp"
#include "automata/detautomaton.hpp"
#include "automata/detstate.hpp"
#include "automata/transition.hpp"

class DetManager {

	using DFAStatesTable = std::unordered_map<BitsetWrapper, DetState*>;
	using VectorCharTable = std::unordered_map<BitsetWrapper, std::vector<char>>;

 public:

	DetManager(std::string pattern, bool raw_automata=false);

	rematch::Transition* next_transition(DetState* q, char a);

	void computeCaptures(DetState* p, DetState* q, char a);

	char chooseFromCharBitset(BitsetWrapper bs);

	std::shared_ptr<VariableFactory> varFactory() const {return variable_factory_;}
	std::shared_ptr<FilterFactory> filterFactory() const {return filter_factory_;}

	DetAutomaton& DFA() {return *dfa_;}
	ExtendedVA& NFA() {return *nfa_;}

 private:
	// ExtendedVA representation of the given pattern.
	std::unique_ptr<ExtendedVA> nfa_;

	// Determinization of the eVA computed on-the-fly.
	std::unique_ptr<DetAutomaton> dfa_;

	// Access to regex's factories
	std::shared_ptr<VariableFactory> variable_factory_;
	std::shared_ptr<FilterFactory> filter_factory_;

	// Hash table for the mapping between bitstring subset representation of
	// NFA states to DFA states
	DFAStatesTable dstates_table_;

	VectorCharTable all_chars_table_;
};

#endif

