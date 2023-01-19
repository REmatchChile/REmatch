#ifndef FILTER__FACTORY_HPP
#define FILTER__FACTORY_HPP

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <ostream>
#include <bitset>
#include <map>
#include <utility>

#include "parsing/charclass_builder.hpp"
#include "parsing/charclass.hpp"

namespace rematch {
// Maximum variables supported

// Filter code table. Manages the all CharClassBuilders that are associated with
// a query (we call them Filters for shorthand).
class FilterFactory {
 public:
	// Constructors
	FilterFactory();

	size_t size() const {return size_;}

	std::string pprint();

	std::unordered_map<std::vector<bool>, std::vector<char>> allPossibleCharBitsets();

	// Adds a CharClassBuilder to the factor and returns its associated code.
	// If already present then just returns the associated code.
	int add_filter(CharClassBuilder ccb);

	// Given a CharClassBuilder, gets the associated code.
	int get_code(CharClassBuilder ccb);

	// Given a code, gets the associated CharClassBuilder.
	CharClassBuilder& get_filter(int code);

	// Check if a CharClassBuilder is already present in the factory.
	bool contains(CharClassBuilder &ccb) const;

	// Merges the Factory with another one inplace.
	void merge(FilterFactory &rest);

	std::vector<bool> applyFilters(char a);

 private:
	size_t size_ = 0;
	// Ordered vector that stores the variables.
	std::unordered_map<CharClassBuilder, char> code_map_;
	std::unordered_map<char, CharClassBuilder> filter_map_;
	// Char (document-readed) -> Bitset hash table
	std::unordered_map<char, std::vector<bool>> bitsetMap;
};

} // end namespace rematch

#endif
