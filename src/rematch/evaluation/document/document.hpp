#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <algorithm>
#include <cstring>
#include <fstream>
#include <istream>
#include <iterator>
#include <string>

#include "evaluation/document/iterators.hpp"

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

#include "evaluation/document/filedocument.hpp"
#include "evaluation/document/strdocument.hpp"

#endif // DOCUMENT_HPP