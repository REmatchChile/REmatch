#ifndef EVA_HPP
#define EVA_HPP

#include <queue>
#include <string>
#include <bitset>
#include <unordered_map>
#include <set>

#include "bitsetwrapper.hpp"

class LVAState;
class VariableFactory;
class FilterFactory;
class LogicalVA;
class CharClass;
class LVACapture;

// FIXME: Add docstrings and refine declarations.
class ExtendedVA {

public:
	LVAState* initState;

	// TODO: Maybe have only a hash tables instead of vectors
	std::vector<LVAState*> states;
	std::vector<LVAState*> finalStates;
	std::vector<LVAState*> superFinalStates;

	std::unordered_map<unsigned int, LVAState*> idMap;

	VariableFactory *vFact;
	FilterFactory *fFact;

	size_t currentID;

	ExtendedVA(LogicalVA &A);
	ExtendedVA();

	// ~ExtendedVA();

	// Copy constructor for copying states accordingly
	ExtendedVA(const ExtendedVA &extended_automaton);


	void addFilter(LVAState* state, CharClass cs, LVAState* next);

	void addCapture(LVAState* state, std::bitset<32> bs, LVAState* next);

	std::string pprint();

	size_t size();
	void epsilonClosure(LogicalVA &A);
	void utilEpsilonClosure(LVAState *from, LVAState *current);
	void adaptReachableStates(LogicalVA &A);
	void utilCleanUnreachable(LVAState *state);
	void pruneUselessStates();
	void pruneDFS(LVAState *state, std::vector<LVAState*> &tmp);
	void captureClosure();
	void cleanUselessCaptureStates();
	void cleanUselessCaptureTransitions();
	std::queue<LVAState*> invTopologicalSort();
	void invTopologicalSortUtil(LVAState *state, std::queue<LVAState*> *Q);
	void relabelStates();
	void utilRelabelStates(LVAState *state);
	void searchSuperFinals();
	bool utilSearchSuperFinals(LVAState *state);

	std::set<LVAState*> getSubset(BitsetWrapper bs) const;

	// Returns a vector of pointers to the LVACaptures present in the automaton
	// sorted accoding to an inverse topological sort for the automaton graph.
	// Used for capture offset optimization.
	std::vector<std::shared_ptr<LVACapture>> getInvTopSortCaptures();

	std::vector<std::list<std::shared_ptr<LVACapture>>> classifySingleCaptures();

	bool offsetPossible(std::shared_ptr<LVACapture> capture);
	bool offsetPossible(std::list<std::shared_ptr<LVACapture>> &captureList);

	void computeOffset(std::list<std::shared_ptr<LVACapture>> &captureList, int codeIndex);

	void offsetOpt();
};

#endif
