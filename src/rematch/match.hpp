#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include <map>
#include <utility>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <memory>

#include "factories/factories.hpp"

namespace rematch {

class Match;
class Enumerator;
class ECS;
class StrDocument;
// FIXME: Change this
class Interface;

#ifdef SWIG
using Match_ptr = Match*;
#else
using Match_ptr = std::unique_ptr<Match>;
#endif


using Span = std::pair<int64_t,int64_t>;
using SpanMap = std::map<std::string, Span>;
using SpanVect = std::vector<Span>;

// Represents a match for easy access to the captured spans and substrings.
// It doesn't store the correspondings substrings, so it's assumed that the
// sublaying document is available.
class Match {
  friend class Enumerator;
  friend class ECS;
  friend class Interface;

 public:

  Match() = default;

  Match(std::shared_ptr<VariableFactory> vf, std::vector<int64_t> m)
      : data_(m), var_factory_(vf) {}

  operator bool() const {return !data_.empty();}

  int64_t start(std::string varname) const;
  int64_t end(std::string varname) const;

  Span span(std::string var) const;

  // Returns a variable's captured substring
  std::string group(std::string var, std::shared_ptr<StrDocument>& doc) const;

  // Returns referece to the sublaying document.
  const std::string& doc() const;

  // Returns a vector with the variable names in order
  std::vector<std::string> variables() const;

  SpanMap& data();

  std::string pprint(std::shared_ptr<StrDocument>& doc) const;

  friend std::ostream& operator<<(std::ostream &os, Match &m);

 private:


  // Enumerator needs to access data_ to fill out the mappings
  void set_mapping(int var_code, int64_t pos) {data_[var_code] = pos;}

  // No advantage in using STL containers like std::map and std::unordered_map.
  std::vector<int64_t> data_;
  // Access to variable names
  std::shared_ptr<VariableFactory> var_factory_;

}; // end class Match

} // end namespace rematch


#endif // MATCH_HPP