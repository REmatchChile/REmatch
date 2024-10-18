#include "parsing/variable_catalog.hpp"
#include <REmatch/exceptions.hpp>

namespace REmatch {
inline namespace parsing {

VariableCatalog::VariableCatalog() {}

std::string VariableCatalog::get_var(uint32_t position) {
  return variables_.at(position);
}

int VariableCatalog::position(std::string var) const {
  auto it = std::lower_bound(variables_.begin(), variables_.end(), var);

  if (it != variables_.end() && var >= *it) {
    return it - variables_.begin();
  }

  throw REmatch::VariableNotFoundInCatalogException(var);
}

void VariableCatalog::add(std::string var) {
  if (size() >= MAX_VARS)
    return;

  auto it = std::lower_bound(variables_.begin(), variables_.end(), var);

  if (it == variables_.end() || var < *it) {
    variables_.insert(it, var);
    // Add offsetMap new entries, defaults to zero.
    offsetMap.push_back(0);
    offsetMap.push_back(0);
  }
}

std::bitset<64> VariableCatalog::open_code(std::string var) {
  std::bitset<64> bitstring;

  auto it = std::lower_bound(variables_.begin(), variables_.end(), var);

  if (it != variables_.end() && var >= *it) {
    bitstring.set((it - variables_.begin()) * 2);
  }

  return bitstring;
}

std::bitset<64> VariableCatalog::close_code(std::string var) {
  std::bitset<64> bitstring;

  auto it = std::lower_bound(variables_.begin(), variables_.end(), var);

  if (it != variables_.end() && var >= *it) {
    bitstring.set((it - variables_.begin()) * 2 + 1);
  }

  return bitstring;
}

std::string VariableCatalog ::print_varset(std::bitset<64> code) {
  std::stringstream ss;

  // Get a container just for printing output correctly with commas
  std::vector<std::string> container;

  for (size_t i = 0; i < 16; i++) {
    if (code[2 * i]) {
      ss << variables_[i];
      if (offsetMap[2 * i])
        ss << "(-" << offsetMap[2 * i] << ")";
      ss << "<";
      container.push_back(ss.str());
      ss.str(std::string());  // Erase the stringstream
    }
    if (code[2 * i + 1]) {
      ss << ">" << variables_[i];
      if (offsetMap[2 * i + 1])
        ss << "(-" << offsetMap[2 * i + 1] << ")";
      container.push_back(ss.str());
      ss.str(std::string());  // Erase the stringstream
    }
  }

  if (!container.empty()) {
    ss << container[0];
    for (size_t i = 1; i < container.size(); i++) {
      ss << "" << container[i];
    }
  }

  return ss.str();
}

std::string VariableCatalog::pprint() {
  std::stringstream ss;
  for (size_t i = 0; i < size(); i++) {
    ss << variables_[i] << " -> " << i << "\n";
  }

  return ss.str();
}

void VariableCatalog::merge(VariableCatalog& rhs) {
  for (auto& var : rhs.variables_) {
    auto it = std::lower_bound(variables_.begin(), variables_.end(), var);
    if (size() >= MAX_VARS) {
      throw REmatch::VariableLimitExceededException();
    }
    if (it == variables_.end()) {
      variables_.insert(it, var);
      offsetMap.push_back(0);
      offsetMap.push_back(0);
    } else if (*it != var) {
      variables_.insert(it, var);
    }
  }
}

void VariableCatalog::merge_disjoint(VariableCatalog& rhs) {
  for (auto& var : rhs.variables_) {
    auto it = std::lower_bound(variables_.begin(), variables_.end(), var);
    if (size() >= MAX_VARS) {
      throw REmatch::VariableLimitExceededException();
    }
    if (it == variables_.end()) {
      variables_.insert(it, var);
      offsetMap.push_back(0);
      offsetMap.push_back(0);
    } else if (*it != var) {
      variables_.insert(it, var);
    } else {
      throw REmatch::MultiSpannersNotAllowedException();
    }
  }
}

bool VariableCatalog::contains(std::string var) {
  return std::binary_search(variables_.begin(), variables_.end(), var);
}

bool VariableCatalog::empty() {
  return variables_.empty();
}

bool VariableCatalog::operator==(const VariableCatalog& vf) const {
  return variables_ == vf.variables_;
}

}  // namespace parsing
}  // namespace REmatch
