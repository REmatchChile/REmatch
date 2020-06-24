#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include <map>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace rematch {


using Span = std::pair<size_t, size_t>;
using SpanMap = std::map<std::string, Span>;
using SpanVect = std::vector<std::pair<size_t,size_t>>;

// Represents a match for easy access to the captured spans and substrings.
// It doesn't store the correspondings substrings, so it's assumed that the
// sublaying document is available.
class Match {

 public:

  Match() = default;

  operator bool() const {return !data_.empty();}

  Match(std::string *d): doc_(d) {}

  Match(std::string *d, SpanMap s): doc_(d), data_(s) {}

  Match(std::string *d, SpanVect s, std::vector<std::string> output_scheme): doc_(d) {
    for(size_t i=0; i < output_scheme.size(); i++) {
      data_[output_scheme[i]] = std::make_pair(s[i].first, s[i].second);
    }
  }

  size_t start(std::string varname) const {return span(varname).first;}
  size_t end(std::string varname) const {return span(varname).second;}

  // Returns a std::pair<uint64_t, uint64_t> correspoding to a variable's span.
  Span span(std::string var) const;

  // Returns a variable's captured substring
  std::string group(std::string var) const;

  // Returns referece to the sublaying document.
  std::string * doc() const {return doc_;}

  // Returns a vector with the variable names in order
  std::vector<std::string> variables() const;

  SpanMap & data() {return data_;}

  std::string print();

  friend std::ostream& operator<<(std::ostream &os, Match &m);

 private:

  // If var already in table raise exception
  void addMapping(std::string var, Span span) {
    auto search = data_.find(var);
    if(search == data_.end())
      data_[var] = span;
    else
      throw std::logic_error("Variable already used.");
  }

  std::string *doc_;     // Access to context document.
  SpanMap data_;          // Mappings table.

}; // end class Match

} // end namespace rematch


#endif // MATCH_HPP