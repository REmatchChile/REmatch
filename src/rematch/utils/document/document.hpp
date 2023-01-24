#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <string>
#include <istream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iterator>

#include "utils/document/iterators.hpp"

namespace rematch {

class Document {
 public:
	virtual ~Document() = default;

	virtual CharIterator begin() const = 0;
	virtual EndCharIterator end() const = 0;

	virtual LineIterator line_begin() const = 0;
	virtual EndLineIterator line_end() const = 0;
}; // end class Document

} // end namespace rematch

#endif // DOCUMENT_HPP
