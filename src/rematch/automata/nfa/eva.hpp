#ifndef SRC_REMATCH_AUTOMATA_NFA_EVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_EVA_HPP

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

// @brief An eVA that builds its graph from a VA.
class ExtendedVA {

 public:
	std::vector<State*> states;

	std::unordered_map<unsigned int, State*> idMap;

	ExtendedVA(LogicalVA const &A);
	ExtendedVA();

	// ~ExtendedVA();

	State* initState() const {return init_state_;}

	void set_initial(State* s) {init_state_ = s;}

	void set_accepting(State* s) { accepting_state_ = s; }

	std::shared_ptr<VariableFactory> varFactory() const {return variable_factory_;}
	std::shared_ptr<FilterFactory> filterFactory() const {return filter_factory_;}

	void addCapture(State* state, std::bitset<32> bs, State* next);

	void trim();

	size_t size() const;

	void captureClosure();

	void cleanUselessCaptureStates();
	void cleanUselessCaptureTransitions();

	std::queue<State*> invTopologicalSort();
	void invTopologicalSortUtil(State *state, std::queue<State*> *Q);

	void relabelStates();
	void utilRelabelStates(State *state);

	std::set<State*> getSubset(BitsetWrapper bs) const;

	// @brief Computes the inverse topological sort of the captures.
	// @return A vector of the reverse topological sorting of all capture
	//         transitions
	CaptureVector inverse_topological_sort_captures();

	std::vector<CaptureList> classifySingleCaptures();

	bool offsetPossible(CapturePtr capture);
	bool offsetPossible(CapturePtr capture1, CapturePtr capture2);
	bool offsetPossible(CaptureList &captureList);

	void computeOffset(CaptureList &captureList, int codeIndex);

	void offsetOpt();

	void crossProdOpt();

	friend std::ostream& operator<<(std::ostream& os, ExtendedVA const &A);

 private:

	void getInvTopSortCapturesUtil(CapturePtr &cap, CaptureVector &L);
	CaptureVector reachableCaptures(CapturePtr &cap);
	bool isReachable(State* from, State* to);

	State* init_state_;
	State* accepting_state_;

	std::shared_ptr<VariableFactory> variable_factory_;
	std::shared_ptr<FilterFactory> filter_factory_;

	size_t currentID = 0;
};

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_EVA_HPP
