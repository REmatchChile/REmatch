#include "match.hpp"

#include <sstream>

namespace rematch {

Match::Match(std::string const &d, SpanVect s,
             std::vector<std::string> output_scheme)
    : doc_(d) {
  for(size_t i=0; i < output_scheme.size(); i++) {
    data_[output_scheme[i]] = std::make_pair(s[i].first, s[i].second);
  }
}


std::string Match::print() {
  std::stringstream ss;
  for(const auto & elem: data()) {
    ss << elem.first << " = |" << elem.second.first << ',' << elem.second.second << ">\t";
  }

  return ss.str();
}

Span Match::span(std::string var) const {
  auto search = data_.find(var);

  if (search != data_.end())
    return (*search).second;

  throw std::logic_error("No mapping assigned to variable.");
}

std::string Match::group(std::string var) const {
  auto search = data_.find(var);

  if (search != data_.end()) {
    auto &span = (*search).second;
    return doc_.substr(span.first, span.second - span.first);
  }

  throw std::logic_error("No mapping assigned to variable.");
}

std::vector<std::string> Match::variables() const {
  std::vector<std::string> ret;
  for(auto elem: data_)
    ret.push_back(elem.first);

  return ret;
}

void Match::addMapping(std::string var, Span span) {
  auto search = data_.find(var);
    if(search == data_.end())
      data_[var] = span;
    else
      throw std::logic_error("Variable already used.");
}

std::ostream& operator<<(std::ostream &os, Match &m) {
  for(const auto & elem: m.data()) {
    os << elem.first << " = |" << elem.second.first << ',' << elem.second.second << ">\t";
  }

  return os;
}

} // end namespace rematch