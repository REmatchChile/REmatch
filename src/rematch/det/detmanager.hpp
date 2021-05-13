#ifndef DETMANAGER_HPP
#define DETMANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>

#include <boost/functional/hash.hpp> /* hash_combine */

#include "bitsetwrapper.hpp"
#include "det/setstate.hpp"
#include "automata/dfa/dfa.hpp"
#include "automata/dfa/detstate.hpp"
#include "automata/dfa/transition.hpp"
#include "automata/macrodfa/macrodfa.hpp"

namespace rematch {

struct VectorHasher {
    size_t operator()(const std::vector<size_t> &V) const {
        size_t hash = V.size();
        for(auto &i : V) {
            hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

class DetManager {

	using DFAStatesTable = std::unordered_map<BitsetWrapper, DetState*>;
	using VectorCharTable = std::unordered_map<BitsetWrapper, std::vector<char>>;

	// TODO: A map for now but it should be a hash table
	using MacroStatesTable = std::unordered_map<std::vector<size_t>, MacroState*,
																							VectorHasher>;

 public:

	DetManager(std::string pattern, bool raw_automata=false);

	rematch::Transition* next_transition(DetState* q, char a);

	MacroTransition* next_macro_transition(MacroState *ms, char a);

	MacroState* compute_drop_super_finals(MacroState *ms);
	DetState* compute_drop_super_finals(DetState *ms);

	std::shared_ptr<VariableFactory> varFactory() const {return variable_factory_;}
	std::shared_ptr<FilterFactory> filterFactory() const {return filter_factory_;}

	DFA& dfa() {return *dfa_;}
	ExtendedVA& nfa() {return *nfa_;}
	MacroDFA& mdfa() {return *mdfa_;}

 private:
	void computeCaptures(DetState* p, DetState* q, char a);

	char chooseFromCharBitset(BitsetWrapper bs);
	// ExtendedVA representation of the given pattern.
	std::unique_ptr<ExtendedVA> nfa_;

	// Determinization of the eVA computed on-the-fly.
	std::unique_ptr<DFA> dfa_;

	std::unique_ptr<MacroDFA> mdfa_;

	// Access to regex's factories
	std::shared_ptr<VariableFactory> variable_factory_;
	std::shared_ptr<FilterFactory> filter_factory_;

	// Hash table for the mapping between bitstring subset representation of
	// NFA states to DFA states
	DFAStatesTable dstates_table_;

	MacroStatesTable mstates_table_;

	VectorCharTable all_chars_table_;
};

} // end namespace rematch

#endif



