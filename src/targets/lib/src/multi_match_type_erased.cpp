#include "REmatch/multi_match_type_erased.hpp"

#include "match/multi_match.hpp"

namespace REmatch {

MultiMatchTypeErased::MultiMatchTypeErased(std::unique_ptr<MultiMatch> match)
    : self(std::move(match)) {}

MultiMatchTypeErased::MultiMatchTypeErased(const MultiMatchTypeErased& other)
    : self(other.self->clone()) {}

MultiMatchTypeErased& MultiMatchTypeErased::operator=(const MultiMatchTypeErased& other) {
  if (this != &other) {
    self = other.self->clone();
  }
  return *this;
}

MultiMatchTypeErased::MultiMatchTypeErased(MultiMatchTypeErased&& other) noexcept
    : self(std::move(other.self)) {}

MultiMatchTypeErased& MultiMatchTypeErased::operator=(MultiMatchTypeErased&& other) noexcept {
  if (this != &other) {
    self = std::move(other.self);
  }
  return *this;
}

MultiMatchTypeErased::~MultiMatchTypeErased() = default;

std::vector<Span> MultiMatchTypeErased::spans(const std::string& variable_name) const {
  return self->spans(variable_name);
}
std::vector<Span> MultiMatchTypeErased::spans(uint_fast32_t variable_id) const {
  return self->spans(variable_id);
}
std::vector<std::string> MultiMatchTypeErased::groups(const std::string& variable_name) const {
  return self->groups(variable_name);
}
std::vector<std::string> MultiMatchTypeErased::groups(uint_fast32_t variable_id) const {
  return self->groups(variable_id);
}

MultiMatchTypeErased MultiMatchTypeErased::submatch(Span span) const {
  return MultiMatchTypeErased(self->submatch(span));
}

std::vector<std::string> MultiMatchTypeErased::variables() const {
  return self->variables();
}
std::string MultiMatchTypeErased::to_string() const {
  return self->to_string();
}
bool MultiMatchTypeErased::empty() const {
  return self->empty();
}

}  // namespace REmatch
