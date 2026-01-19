#include "REmatch/match.hpp"

#include "match/match/match_standard.hpp"
#include "match/match/s_match.hpp"

namespace REmatch {

Match::Match(std::unique_ptr<internal::Match> match) : self(std::move(match)) {}

Match::Match(const Match& other) : self(other.self->clone()) {}

Match::Match(Match&& other) noexcept : self(std::move(other.self)) {}

Match& Match::operator=(const Match& other) {
  if (this != &other) {
    self = other.self->clone();
  }
  return *this;
}

Match& Match::operator=(Match&& other) noexcept {
  if (this != &other) {
    self = std::move(other.self);
  }
  return *this;
}

Match::~Match() = default;

int64_t Match::start(const std::string& variable_name) const {
  return self->start(variable_name);
}

int64_t Match::start(uint_fast32_t variable_id) const {
  return self->start(variable_id);
}

int64_t Match::end(const std::string& variable_name) const {
  return self->end(variable_name);
}

int64_t Match::end(uint_fast32_t variable_id) const {
  return self->end(variable_id);
}

Span Match::span(const std::string& variable_name) const {
  return self->span(variable_name);
}

Span Match::span(uint_fast32_t variable_id) const {
  return self->span(variable_id);
}

std::string Match::group(const std::string& variable_name) const {
  return self->group(variable_name);
}

std::string Match::group(uint_fast32_t variable_id) const {
  return self->group(variable_id);
}

std::map<std::string, Span> Match::groupdict() const {
  return self->groupdict();
}

std::vector<std::string> Match::variables() const {
  return self->variables();
}

std::string Match::to_string() const {
  return self->to_string();
}

bool Match::empty() const {
  return self->empty();
}

}  // namespace REmatch
