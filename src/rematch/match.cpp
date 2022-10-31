#include "match.hpp"
#include "evaluation/document/strdocument.hpp"

#include <memory>
#include <sstream>

namespace rematch {

Span Match::span(std::string var) const {
  try {
    int pos = var_factory_->position(var);
    return std::make_pair(data_[pos * 2], data_[pos * 2 + 1]);
  } catch (...) {
    throw std::logic_error("No mapping assigned to variable.");
  }
}

std::string_view Match::group(std::string var) const {
  try {
    int pos = var_factory_->position(var);
    return document_.substr(data_[pos * 2], data_[pos * 2 + 1]);
  } catch (...) {
    throw std::logic_error("No mapping assigned to variable.");
  }
}

std::vector<std::string> Match::variables() const {
  return var_factory_->variables();
}

std::string Match::pprint() const {
  std::stringstream ss;
  for (size_t i = 0; i < (data_.size() / 2) - 1; i += 2) {
    std::string var = var_factory_->get_var(i);
    ss << var << " = \"" << group(var) << "\" ";
  }

  std::string var = var_factory_->get_var((data_.size() / 2) - 1);
  ss << var << " = \"" << group(var) << "\"";

  return ss.str();
}

std::ostream& operator<<(std::ostream& os, Match& m) {
  for (size_t i = 0; i < m.data_.size() - 2; i += 2) {
    os << m.var_factory_->get_var(i / 2) << " = |" << m.data_[i] << ','
       << m.data_[i + 1] << ">\t";
  }

  os << m.var_factory_->get_var((m.data_.size() - 2) / 2) << " = |"
     << m.data_[m.data_.size() - 2] << ',' << m.data_[m.data_.size() - 1]
     << ">";

  return os;
}

}  // end namespace rematch