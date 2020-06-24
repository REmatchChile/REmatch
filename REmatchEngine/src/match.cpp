#include "match.hpp"

#include <sstream>

namespace rematch {


std::string Match::print() {
  std::stringstream ss;
  for(const auto & elem: data()) {
    ss << elem.first << " = |" << elem.second.first << ',' << elem.second.second << ">\t";
  }

  return ss.str();
}

Span Match::span(std::string var) const{
  auto search = data_.find(var);

  if (search != data_.end())
    return (*search).second;

  throw std::logic_error("No mapping assigned to variable.");
}

std::string Match::group(std::string var) const {
  auto search = data_.find(var);

  if (search != data_.end()) {
    auto &span = (*search).second;
    return doc_->substr(span.first, span.second - span.first);
  }

  throw std::logic_error("No mapping assigned to variable.");
}

std::vector<std::string> Match::variables() const {
  std::vector<std::string> ret;
  for(auto elem: data_)
    ret.push_back(elem.first);

  return ret;
}

std::ostream& operator<<(std::ostream &os, Match &m) {
  for(const auto & elem: m.data()) {
    os << elem.first << " = |" << elem.second.first << ',' << elem.second.second << ">\t";
  }

  return os;
}

} // end namespace rematch