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

class REMATCH_EXPORT Match {
 public:
  explicit Match(std::unique_ptr<internal::Match> match);

  Match(const Match& other);
  Match(Match&& other) noexcept;

  Match& operator=(const Match& other);
  Match& operator=(Match&& other) noexcept;

  ~Match();

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
  std::unique_ptr<internal::Match> self;
};

inline std::ostream& operator<<(std::ostream& os, const Match& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace REmatch
