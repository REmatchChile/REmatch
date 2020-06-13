#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include <map>
#include <utility>
#include <stdexcept>
#include <vector>

namespace rematch {

// Represents a match for easy access to the captured spans and substrings.
// It doesn't store the correspondings substrings, so it's assumed that the
// sublaying document is available.
class Match {

 public:
  using Span = std::pair<size_t, size_t>;
  using SpanMap = std::map<std::string, Span>;
  using SpanVect = std::vector<std::array<size_t, 2>>;

  Match() = default;

  Match(std::string *d): doc_(d) {}

  Match(std::string *d, SpanMap s): doc_(d), data_(s) {}

  Match(std::string *d, SpanVect s, std::vector<std::string> output_scheme): doc_(d) {
    for(size_t i=0; i < output_scheme.size(); i++) {
      size_t a = s[i][0], b = s[i][1];
      data_[output_scheme[i]] = std::make_pair(s[i][0], s[i][1]);
    }
  }

  // Returns a std::pair<uint64_t, uint64_t> correspoding to a variable's span.
  Span getSpan(std::string var) const {
    auto search = data_.find(var);

    if (search != data_.end())
      return (*search).second;

    throw std::logic_error("No mapping assigned to variable.");
  }

  // Returns a variable's captured substring
  std::string getSubstr(std::string var) const {
    auto search = data_.find(var);

    if (search != data_.end()) {
      auto &span = (*search).second;
      return doc_->substr(span.first, span.second - span.first);
    }

    throw std::logic_error("No mapping assigned to variable.");
  }

  // Returns referece to the sublaying document.
  std::string * doc() const {return doc_;}

  SpanMap & data() {return data_;}

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