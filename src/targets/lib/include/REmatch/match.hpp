#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "REmatch/span.hpp"
#include "REmatch_export.hpp"

namespace REmatch {

namespace internal {
class Match;
}

// Represents a result of a query.
class REMATCH_EXPORT Match {
 public:
  explicit Match(std::unique_ptr<internal::Match> match);

  Match(const Match& other);
  Match(Match&& other) noexcept;

  Match& operator=(const Match& other);
  Match& operator=(Match&& other) noexcept;

  ~Match();

  // Retrieves the first index of the span associated to the given variable name.
  int64_t start(const std::string& variable_name) const;

  // Retrieves the first index of the span associated to the given variable id.
  int64_t start(uint_fast32_t variable_id) const;

  // Retrieves the first index of the span associated to the given variable name.
  int64_t end(const std::string& variable_name) const;

  // Retrieves the first index of the span associated to the given variable id.
  int64_t end(uint_fast32_t variable_id) const;

  // Retrieves the span associated to the given variable name.
  Span span(const std::string& variable_name) const;

  // Retrieves the span associated to the given variable id.
  Span span(uint_fast32_t variable_id) const;

  // Retrieves the string captured by the variable.
  std::string group(const std::string& variable_name) const;

  // Retrieves the string captured by the variable.
  std::string group(uint_fast32_t variable_id) const;

  // Returns a map that contains the name of the variables as keys and their corresponding spans
  // as values.
  std::map<std::string, Span> groupdict() const;

  // Returns a vector containing the variables present in the query.
  std::vector<std::string> variables() const;

  // Returns a string representation of the Match.
  std::string to_string() const;

  // Returns true if the match is empty, false otherwise. The match is empty when the query matches
  // the document, but it does not contain any variables.
  bool empty() const;

 private:
  std::unique_ptr<internal::Match> self;
};

inline std::ostream& operator<<(std::ostream& os, const Match& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace REmatch
