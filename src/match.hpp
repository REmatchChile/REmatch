#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include <map>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>

namespace rematch {

class Match;

#ifdef SWIG
using Match_ptr = Match*;
#else
using Match_ptr = std::unique_ptr<Match>;
#endif


using Span = std::pair<int64_t, int64_t>;
using SpanMap = std::map<std::string, Span>;
using SpanVect = std::vector<Span>;

// Represents a match for easy access to the captured spans and substrings.
// It doesn't store the correspondings substrings, so it's assumed that the
// sublaying document is available.
class Match {
 public:
  operator bool() const {return !data_.empty();}

  Match(std::string const &d): doc_(d) {}

  Match(std::string const &d, SpanMap s): doc_(d), data_(s) {}

  Match(std::string const &d, SpanVect s,
        std::vector<std::string> output_scheme);

  int64_t start(std::string varname) const {return span(varname).first;}
  int64_t end(std::string varname) const {return span(varname).second;}

  // Returns a pair<uint64_t, uint64_t> correspoding to a variable's span.
  Span span(std::string var) const;

  // Returns a variable's captured substring
  std::string group(std::string var) const;

  // Returns referece to the sublaying document.
  const std::string& doc() const {return doc_;}

  // Returns a vector with the variable names in order
  std::vector<std::string> variables() const;

  SpanMap& data() {return data_;}

  std::string print();

  friend std::ostream& operator<<(std::ostream &os, Match &m);

 private:

  // If var already in table raise exception
  void addMapping(std::string var, Span span);


  const std::string &doc_;     // Access to context document.
  SpanMap data_;          // Mappings table.

}; // end class Match

} // end namespace rematch


#endif // MATCH_HPP