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

class REMATCH_EXPORT MultiMatch {
 public:
  explicit MultiMatch(std::unique_ptr<internal::MultiMatch> match);

  MultiMatch(const MultiMatch& other);

  MultiMatch& operator=(const MultiMatch& other);

  MultiMatch(MultiMatch&& other) noexcept;

  MultiMatch& operator=(MultiMatch&& other) noexcept;

  ~MultiMatch();

  std::vector<Span> spans(const std::string& variable_name) const;
  std::vector<Span> spans(uint_fast32_t variable_id) const;
  std::vector<std::string> groups(const std::string& variable_name) const;
  std::vector<std::string> groups(uint_fast32_t variable_id) const;
  MultiMatch submatch(Span span) const;
  std::vector<std::string> variables() const;
  std::string to_string() const;
  bool empty() const;

 private:
  std::unique_ptr<internal::MultiMatch> self;
};

inline std::ostream& operator<<(std::ostream& os, const MultiMatch& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace REmatch
