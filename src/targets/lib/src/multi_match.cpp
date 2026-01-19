#include "REmatch/multi_match.hpp"

#include "match/match/multi_match_standard.hpp"
#include "match/match/s_multi_match.hpp"

namespace REmatch {

MultiMatch::MultiMatch(std::unique_ptr<internal::MultiMatch> match)
    : self(std::move(match)) {}

MultiMatch::MultiMatch(const MultiMatch& other)
    : self(other.self->clone()) {}

MultiMatch& MultiMatch::operator=(const MultiMatch& other) {
  if (this != &other) {
    self = other.self->clone();
  }
  return *this;
}

MultiMatch::MultiMatch(MultiMatch&& other) noexcept
    : self(std::move(other.self)) {}

MultiMatch& MultiMatch::operator=(MultiMatch&& other) noexcept {
  if (this != &other) {
    self = std::move(other.self);
  }
  return *this;
}

MultiMatch::~MultiMatch() = default;

std::vector<Span> MultiMatch::spans(const std::string& variable_name) const {
  return self->spans(variable_name);
}
std::vector<Span> MultiMatch::spans(uint_fast32_t variable_id) const {
  return self->spans(variable_id);
}
std::vector<std::string> MultiMatch::groups(const std::string& variable_name) const {
  return self->groups(variable_name);
}
std::vector<std::string> MultiMatch::groups(uint_fast32_t variable_id) const {
  return self->groups(variable_id);
}

MultiMatch MultiMatch::submatch(Span span) const {
  return MultiMatch(self->submatch(span));
}

std::vector<std::string> MultiMatch::variables() const {
  return self->variables();
}
std::string MultiMatch::to_string() const {
  return self->to_string();
}
bool MultiMatch::empty() const {
  return self->empty();
}

}  // namespace REmatch
