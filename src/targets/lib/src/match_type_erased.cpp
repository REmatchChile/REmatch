#include "REmatch/match_type_erased.hpp"

#include "match/match.hpp"

using namespace REmatch;

MatchTypeErased::MatchTypeErased(std::unique_ptr<Match> match) : self(std::move(match)) {}

MatchTypeErased::MatchTypeErased(const MatchTypeErased& other) : self(other.self->clone()) {}

MatchTypeErased::MatchTypeErased(MatchTypeErased&& other) noexcept : self(std::move(other.self)) {}

MatchTypeErased& MatchTypeErased::operator=(const MatchTypeErased& other) {
  if (this != &other) {
    self = other.self->clone();
  }
  return *this;
}

MatchTypeErased& MatchTypeErased::operator=(MatchTypeErased&& other) noexcept {
  if (this != &other) {
    self = std::move(other.self);
  }
  return *this;
}

MatchTypeErased::~MatchTypeErased() = default;

int64_t MatchTypeErased::start(const std::string& variable_name) const {
  return self->start(variable_name);
}

int64_t MatchTypeErased::start(uint_fast32_t variable_id) const {
  return self->start(variable_id);
}

int64_t MatchTypeErased::end(const std::string& variable_name) const {
  return self->end(variable_name);
}

int64_t MatchTypeErased::end(uint_fast32_t variable_id) const {
  return self->end(variable_id);
}

Span MatchTypeErased::span(const std::string& variable_name) const {
  return self->span(variable_name);
}

Span MatchTypeErased::span(uint_fast32_t variable_id) const {
  return self->span(variable_id);
}

std::string MatchTypeErased::group(const std::string& variable_name) const {
  return self->group(variable_name);
}

std::string MatchTypeErased::group(uint_fast32_t variable_id) const {
  return self->group(variable_id);
}

std::map<std::string, Span> MatchTypeErased::groupdict() const {
  return self->groupdict();
}

std::vector<std::string> MatchTypeErased::variables() const {
  return self->variables();
}

std::string MatchTypeErased::to_string() const {
  return self->to_string();
}

bool MatchTypeErased::empty() const {
  return self->empty();
}
