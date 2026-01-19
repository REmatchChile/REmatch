#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "REmatch/span.hpp"

namespace REmatch {

class MatchStandard;
class SMatch;
class Match;

class MatchTypeErased {
 public:
  explicit MatchTypeErased(std::unique_ptr<Match> match);

  MatchTypeErased(const MatchTypeErased& other);
  MatchTypeErased(MatchTypeErased&& other) noexcept;

  MatchTypeErased& operator=(const MatchTypeErased& other);
  MatchTypeErased& operator=(MatchTypeErased&& other) noexcept;

  ~MatchTypeErased();

  int64_t start(const std::string& variable_name) const;
  int64_t start(uint_fast32_t variable_id) const;
  int64_t end(const std::string& variable_name) const;
  int64_t end(uint_fast32_t variable_id) const;
  Span span(const std::string& variable_name) const;
  Span span(uint_fast32_t variable_id) const;
  std::string group(const std::string& variable_name) const;
  std::string group(uint_fast32_t variable_id) const;

  std::map<std::string, Span> groupdict() const;
  std::vector<std::string> variables() const;
  std::string to_string() const;
  bool empty() const;

 private:
  std::unique_ptr<Match> self;
};

inline std::ostream& operator<<(std::ostream& os, const MatchTypeErased& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace REmatch
