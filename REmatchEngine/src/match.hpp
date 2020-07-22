#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include <map>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

namespace rematch {

class Match;

#ifdef SWIG
using Match_ptr = Match*;
#else
using Match_ptr = unique_ptr<Match>;
#endif


using Span = pair<size_t, size_t>;
using SpanMap = map<string, Span>;
using SpanVect = vector<Span>;

// Represents a match for easy access to the captured spans and substrings.
// It doesn't store the correspondings substrings, so it's assumed that the
// sublaying document is available.
class Match {
 public:
  operator bool() const {return !data_.empty();}

  Match(string const &d): doc_(d) {}

  Match(string const &d, SpanMap s): doc_(d), data_(s) {}

  Match(string const &d, SpanVect s,
        vector<string> output_scheme);

  size_t start(string varname) const {return span(varname).first;}
  size_t end(string varname) const {return span(varname).second;}

  // Returns a pair<uint64_t, uint64_t> correspoding to a variable's span.
  Span span(string var) const;

  // Returns a variable's captured substring
  string group(string var) const;

  // Returns referece to the sublaying document.
  const string& doc() const {return doc_;}

  // Returns a vector with the variable names in order
  vector<string> variables() const;

  SpanMap& data() {return data_;}

  string print();

  friend ostream& operator<<(ostream &os, Match &m);

 private:

  // If var already in table raise exception
  void addMapping(string var, Span span);


  const string &doc_;     // Access to context document.
  SpanMap data_;          // Mappings table.

}; // end class Match

} // end namespace rematch


#endif // MATCH_HPP