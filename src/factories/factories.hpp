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

#include "parser/ast.hpp"
#include "bitsetwrapper.hpp"
#include "charclass.hpp"

namespace rematch {

// Maximum variables supported
const int MAX_VARS = 16;

class VariableFactory {
/*
 * Manager of the parsed RGX formula variables where each variable name
 * has a corresponding code.
 */
	private:

		// Total vars present at factory
		size_t numVars;

		// VarName -> VarCode hash table
		std::unordered_map<std::string, unsigned int> codeMap;

		// VarCode -> VarName hash table
		std::unordered_map<unsigned int, std::string> varMap;

		// Offset capturing optimization. Maps each opening and closing
		// capture variable to its computed offset. Then it's a vector of size
		// numVars*2. The vector is such that:
		// 		- offsetMap[2*c]         is the opening (x<) offset.
		//    - offsetMap[2*c + 1] 		 is the closing (x>) offset.
		std::vector<int> offsetMap;

		bool computedOffsets; // True iff offsetMap was computed.

	public:

		// Simple constructor
		VariableFactory();

		size_t size() {return numVars;}

		std::string getVarName(unsigned int position);

		// Add a variable to the struct
		void addVar(std::string varName);

		// Given a variable name outputs the corresponding opening bitset
		std::bitset<32> getOpenCode(std::string varName);

		// Given a variable name outputs the corresponding closing bitset
		std::bitset<32> getCloseCode(std::string varName);

		// Given a bitset outputs the corresponding opening and closing variables
		// as a std::string
		std::string getVarUtil(std::bitset<32> code);

		std::vector<std::string> getOutputSchema();

		// Prints the hash table
		std::string pprint();

		// Merges the variables present in another VariableFactory inplace
		void merge(VariableFactory &rhs);

		// Checks if a variable name is present
		bool isMember(std::string varName);

		bool isEmpty();

		// Equality operator overload
		bool operator ==(const VariableFactory &vf) const;

		int& getOffset(int index) {return offsetMap[index];}
};



class FilterFactory {

	private:

		// Total number of parsed filters
		size_t numFilters;

		// CharClass -> Code hash table
		std::unordered_map<CharClass, int> codeMap;

		// Code -> CharClass hash table
		std::unordered_map<int, CharClass> filterMap;

		// Char (document-readed) -> Bitset hash table
		std::unordered_map<char, BitsetWrapper> bitsetMap;

	public:

		// Constructors
		FilterFactory();
		FilterFactory(const ast::charset &cs);
		FilterFactory(const char &a);
		FilterFactory(special spec, bool negated);

		size_t size() {return numFilters;}

		std::string pprint();

		std::unordered_map<BitsetWrapper, std::vector<char>> allPossibleCharBitsets();

		bool inIntersection(char a, BitsetWrapper charBitset);

		void addFilter(CharClass cs);

		int getCode(CharClass cs);

		CharClass getFilter(int code);

		bool isMember(CharClass cs);

		void merge(FilterFactory &rest);

		BitsetWrapper applyFilters(char a);
};

} // end namespace rematch

#endif