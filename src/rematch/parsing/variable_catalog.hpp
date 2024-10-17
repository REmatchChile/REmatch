#pragma once

#include <algorithm>
#include <bitset>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace REmatch {
inline namespace parsing {

const int MAX_VARS = 15;

// Manager of the parsed RGX formula variables where each variable name
// has a corresponding code.
class VariableCatalog {
 public:
	// Simple constructor
	VariableCatalog();

	size_t size() {return variables_.size();}

	std::string get_var(uint32_t position);

	std::vector<std::string> variables() {return variables_;}

	int position(std::string var) const;

	// Add a variable to the struct
	void add(std::string var);

	// Given a variable name outputs the corresponding opening bitset
	std::bitset<64> open_code(std::string var);

	// Given a variable name outputs the corresponding closing bitset
	std::bitset<64> close_code(std::string var);

	// Given a bitset outputs the corresponding opening and closing variables
	// as a std::string
	std::string print_varset(std::bitset<64> code);

	// Prints the hash table
	std::string pprint();

	// Merges the variables present in another VariableCatalog inplace
	void merge(VariableCatalog &rhs);

	void merge_disjoint(VariableCatalog &rhs);

	// Checks if a variable name is present
	bool contains(std::string var);

	bool empty();

	// Equality operator overload
	bool operator==(const VariableCatalog &vf) const;

	int& get_offset(int index) {return offsetMap[index];}

 private:
	// Ordered vector that stores the variables.
	std::vector<std::string> variables_;

	// Offset capturing optimization. Maps each opening and closing
	// capture variable to its computed offset. Then it's a vector of size
	// numVars*2. The vector is such that:
	// 		- offsetMap[2*c]         is the opening (x<) offset.
	//    - offsetMap[2*c + 1] 		 is the closing (x>) offset.
	std::vector<int> offsetMap;
};

}
}
