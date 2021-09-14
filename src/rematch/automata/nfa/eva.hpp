#ifndef EVA_HPP
#define EVA_HPP

#include <queue>
#include <string>
#include <bitset>
#include <unordered_map>
#include <set>
#include <list>

#include "bitsetwrapper.hpp"

namespace rematch {

class State;
class VariableFactory;
class FilterFactory;
class LogicalVA;
class CharClass;
class LVACapture;

using CaptureVector = std::vector<std::shared_ptr<LVACapture>>;
using CaptureList = std::list<std::shared_ptr<LVACapture>>;
using CapturePtr = std::shared_ptr<LVACapture>;

// FIXME: Add docstrings and refine declarations.
class ExtendedVA {

 public:

	// TODO: Maybe have only a hash tables instead of vectors
	std::vector<State*> states;
	std::vector<State*> finalStates;
	std::vector<State*> superFinalStates;

	std::unordered_map<unsigned int, State*> idMap;

	ExtendedVA(LogicalVA &A);
	ExtendedVA();

	// ~ExtendedVA();

	// FIXME: Not working properly
	// Copy constructor for copying states accordingly
	ExtendedVA(const ExtendedVA &extended_automaton);

	State* initState() const {return init_state_;}
	void set_initState(State* s) {init_state_ = s;}
	std::shared_ptr<VariableFactory> varFactory() const {return variable_factory_;}
	std::shared_ptr<FilterFactory> filterFactory() const {return filter_factory_;}

	void addCapture(State* state, std::bitset<32> bs, State* next);

	std::string pprint();

	size_t size();

	// Compute the epsilon closure correcty. Takes the transitions forward so if
	// 				q -[eps]-> p -[a]-> r
	// Then:
	//				q 				 p -[a]-> r
	//				ᴸ--------[a]--------^
	void epsilonClosure(LogicalVA &A);

	void adaptReachableStates(LogicalVA &A);
	void utilCleanUnreachable(State *state);
	void pruneUselessStates();
	void pruneDFS(State *state, std::vector<State*> &tmp);
	void captureClosure();
	void cleanUselessCaptureStates();
	void cleanUselessCaptureTransitions();
	std::queue<State*> invTopologicalSort();
	void invTopologicalSortUtil(State *state, std::queue<State*> *Q);
	void relabelStates();
	void utilRelabelStates(State *state);
	void searchSuperFinals();
	bool utilSearchSuperFinals(State *state);

	std::set<State*> getSubset(BitsetWrapper bs) const;

	// Returns a vector of pointers to the LVACaptures present in the automaton
	// sorted accoding to an inverse topological sort for the automaton graph.
	// Used for capture offset optimization.
	CaptureVector getInvTopSortCaptures();

	std::vector<CaptureList> classifySingleCaptures();

	bool offsetPossible(CapturePtr capture);
	bool offsetPossible(CapturePtr capture1, CapturePtr capture2);
	bool offsetPossible(CaptureList &captureList);

	void computeOffset(CaptureList &captureList, int codeIndex);

	void offsetOpt();

	void crossProdOpt();

	void compute_if_dfa_searchable();

	bool is_dfa_searchable() const {return is_dfa_searchable_;}


 private:
	void getInvTopSortCapturesUtil(CapturePtr &cap, CaptureVector &L);
	CaptureVector reachableCaptures(CapturePtr &cap);
	bool isReachable(State* from, State* to);

	State* init_state_;

	std::shared_ptr<VariableFactory> variable_factory_;
	std::shared_ptr<FilterFactory> filter_factory_;

	size_t currentID;

	bool is_raw_;
	bool is_dfa_searchable_ = false;

};

} // end namespace rematch

#endif
