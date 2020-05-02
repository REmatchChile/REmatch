#ifndef DETMANAGER_HPP
#define DETMANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>
#include <array>

#include "bitsetwrapper.hpp"
#include "structs/vector.hpp"

class SetState;
class DetState;

class ExtendedVA;
class DetAutomaton;

class DetManager {

public:

	ExtendedVA *eVA;
	DetAutomaton *detA;

	std::unordered_map<BitsetWrapper, DetState*> setStatesMap;

	// Array of vectors that need to resize given DetManager's command
	std::array<rematch::vector<DetState*>*, 3> dependentVectors;

	SetState* initSetState;

	size_t numDetStatesThreshold;

  bool vector_connection;

  std::unordered_map<BitsetWrapper, std::vector<char>> allCharBitsets;

	DetManager(ExtendedVA*, DetAutomaton*, std::array<rematch::vector<DetState*>*, 3>);
  DetManager(ExtendedVA*, DetAutomaton*);

	DetState* getNextSubset(SetState* ss, BitsetWrapper charBitset);
	void computeCaptures(DetState* q);
	void computeFullDetAutomaton();
  void computeFullDetAutomatonSingles();
	DetState* getNextDetState(DetState* s, BitsetWrapper charBitset);
	DetState* getNextDetState(DetState* &s, char a, size_t idx);
	DetState* getNextDetState(DetState* s, char a);
	BitsetWrapper applyFilters(char a);

	std::string uniformSample(size_t n);
	char chooseFromCharBitset(BitsetWrapper bs);
};

#endif

