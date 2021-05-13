#ifndef FACTORIES_HPP
#define FACTORIES_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <ostream>
#include <bitset>
#include <map>
#include <utility>

#include "parse/regex/ast.hpp"
#include "bitsetwrapper.hpp"
#include "charclass.hpp"

namespace rematch {

// Maximum variables supported
const int MAX_VARS = 16;

// Manager of the parsed RGX formula variables where each variable name
// has a corresponding code.
class VariableFactory {
 public:
	// Simple constructor
	VariableFactory();

	size_t size() {return data_.size();}

	std::string get_var(uint position);

	std::vector<std::string> variables() {return data_;}

	int position(std::string var) const;

	// Add a variable to the struct
	void add(std::string var);

	// Given a variable name outputs the corresponding opening bitset
	std::bitset<32> open_code(std::string var);

	// Given a variable name outputs the corresponding closing bitset
	std::bitset<32> close_code(std::string var);

	// Given a bitset outputs the corresponding opening and closing variables
	// as a std::string
	std::string print_varset(std::bitset<32> code);

	// Prints the hash table
	std::string pprint();

	// Merges the variables present in another VariableFactory inplace
	void merge(VariableFactory &rhs);

	// Checks if a variable name is present
	bool contains(std::string var);

	bool empty();

	// Equality operator overload
	bool operator ==(const VariableFactory &vf) const;

	int& get_offset(int index) {return offsetMap[index];}

 private:
	// Ordered vector that stores the variables.
	std::vector<std::string> data_;

	// Offset capturing optimization. Maps each opening and closing
	// capture variable to its computed offset. Then it's a vector of size
	// numVars*2. The vector is such that:
	// 		- offsetMap[2*c]         is the opening (x<) offset.
	//    - offsetMap[2*c + 1] 		 is the closing (x>) offset.
	std::vector<int> offsetMap;

	bool computedOffsets; // True iff offsetMap was computed.
};



class FilterFactory {
 public:
	// Constructors
	FilterFactory();

	size_t size() {return numFilters;}

	std::string pprint();

	std::unordered_map<BitsetWrapper, std::vector<char>> allPossibleCharBitsets();

	bool inIntersection(char a, BitsetWrapper charBitset);

	int addFilter(CharClass cs);

	int getCode(CharClass cs);

	CharClass getFilter(int code);

	bool isMember(CharClass cs);

	void merge(FilterFactory &rest);

	BitsetWrapper applyFilters(char a);

 private:
	// Total number of parsed filters
	size_t numFilters;
	// CharClass -> Code hash table
	std::unordered_map<CharClass, int> codeMap;
	// Code -> CharClass hash table
	std::unordered_map<int, CharClass> filterMap;
	// Char (document-readed) -> Bitset hash table
	std::unordered_map<char, BitsetWrapper> bitsetMap;
};

} // end namespace rematch

#endif