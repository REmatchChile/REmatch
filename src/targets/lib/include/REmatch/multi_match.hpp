#pragma once

#include <memory>
#include <string>
#include <vector>

#include "REmatch/span.hpp"
#include "REmatch_export.hpp"

namespace REmatch {

namespace internal {
class MultiMatch;
}

// Represents a result of a multi query.
class REMATCH_EXPORT MultiMatch {
 public:
  explicit MultiMatch(std::unique_ptr<internal::MultiMatch> match);

  MultiMatch(const MultiMatch& other);

  MultiMatch& operator=(const MultiMatch& other);

  MultiMatch(MultiMatch&& other) noexcept;

  MultiMatch& operator=(MultiMatch&& other) noexcept;

  ~MultiMatch();

  // Retrieves a vector of spans associated to the variable name.
  std::vector<Span> spans(const std::string& variable_name) const;

  // Retrieves a vector of spans associated to the variable id.
  std::vector<Span> spans(uint_fast32_t variable_id) const;

  // Retrieves a vector of strings captured by the variable.
  std::vector<std::string> groups(const std::string& variable_name) const;

  // Retrieves a vector of strings captured by the variable.
  std::vector<std::string> groups(uint_fast32_t variable_id) const;

  // Computes a multi match that contains the spans inside the span passed as argument.
  MultiMatch submatch(Span span) const;

  // Returns a vector containing the variables present in the query.
  std::vector<std::string> variables() const;

  // Returns a string representation of the MultiMatch.
  std::string to_string() const;

  // Returns true if the multi match is empty, false otherwise. The match is empty when the query
  // matches the document, but it does not contain any variables.
  bool empty() const;

 private:
  std::unique_ptr<internal::MultiMatch> self;
};

inline std::ostream& operator<<(std::ostream& os, const MultiMatch& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace REmatch
