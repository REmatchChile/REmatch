#pragma once

#include <memory>
#include <string>
#include <vector>
#include "REmatch/span.hpp"

namespace REmatch {

class MultiMatch;

class MultiMatchTypeErased {
 public:
  explicit MultiMatchTypeErased(std::unique_ptr<MultiMatch> match);

  MultiMatchTypeErased(const MultiMatchTypeErased& other);

  MultiMatchTypeErased& operator=(const MultiMatchTypeErased& other);

  MultiMatchTypeErased(MultiMatchTypeErased&& other) noexcept;

  MultiMatchTypeErased& operator=(MultiMatchTypeErased&& other) noexcept;

  ~MultiMatchTypeErased();

  std::vector<Span> spans(const std::string& variable_name) const;
  std::vector<Span> spans(uint_fast32_t variable_id) const;
  std::vector<std::string> groups(const std::string& variable_name) const;
  std::vector<std::string> groups(uint_fast32_t variable_id) const;
  MultiMatchTypeErased submatch(Span span) const;
  std::vector<std::string> variables() const;
  std::string to_string() const;
  bool empty() const;

 private:
  std::unique_ptr<MultiMatch> self;
};

inline std::ostream& operator<<(std::ostream& os, const MultiMatchTypeErased& obj) {
  os << obj.to_string();
  return os;
}

}  // namespace REmatch
