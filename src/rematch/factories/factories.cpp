#include <algorithm>
#include <bitset>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "charclass.hpp"
#include "exceptions.hpp"
#include "factories.hpp"
#include "parse/visitors.hpp"

namespace rematch {

VariableFactory::VariableFactory() {}

std::string VariableFactory::get_var(unsigned int position) { return data_[position]; }

int VariableFactory::position(std::string var) const {
  auto it = std::lower_bound(data_.begin(), data_.end(), var);

  if (it != data_.end() && var >= *it) {
    return it - data_.begin();
  } else {
    // TODO: Throw exception
    return -1;
  }
}

void VariableFactory::add(std::string var) {
  if (size() >= MAX_VARS)
    return;

  auto it = std::lower_bound(data_.begin(), data_.end(), var);

  if (it == data_.end() || var < *it) {
    data_.insert(it, var);
    // Add offsetMap new entries, defaults to zero.
    offsetMap.push_back(0);
    offsetMap.push_back(0);
  }
}

std::bitset<32> VariableFactory::open_code(std::string var) {
  std::bitset<32> bitstring;

  auto it = std::lower_bound(data_.begin(), data_.end(), var);

  if (it != data_.end() && var >= *it) {
    bitstring.set((it - data_.begin()) * 2);
  }

  return bitstring;
}

std::bitset<32> VariableFactory::close_code(std::string var) {
  std::bitset<32> bitstring;

  auto it = std::lower_bound(data_.begin(), data_.end(), var);

  if (it != data_.end() && var >= *it) {
    bitstring.set((it - data_.begin()) * 2 + 1);
  }

  return bitstring;
}

std::string VariableFactory ::print_varset(std::bitset<32> code) {
  std::stringstream ss;

  // Get a container just for printing output correctly with commas
  std::vector<std::string> container;

  for (size_t i = 0; i < 16; i++) {
    if (code[2 * i]) {
      ss << data_[i];
      if (offsetMap[2 * i])
        ss << "(-" << offsetMap[2 * i] << ")";
      ss << "<";
      container.push_back(ss.str());
      ss.str(std::string()); // Erase the stringstream
    }
    if (code[2 * i + 1]) {
      ss << ">" << data_[i];
      if (offsetMap[2 * i + 1])
        ss << "(-" << offsetMap[2 * i + 1] << ")";
      container.push_back(ss.str());
      ss.str(std::string()); // Erase the stringstream
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

std::string VariableFactory::pprint() {
  std::stringstream ss;
  for (size_t i = 0; i < size(); i++) {
    ss << data_[i] << " -> " << i << "\n";
  }

  return ss.str();
}

void VariableFactory ::merge(VariableFactory &rhs) {
  for (auto &var : rhs.data_) {
    auto it = std::lower_bound(data_.begin(), data_.end(), var);
    if (it != data_.end() && var == *it) {
      throw parsing::BadRegex("Regex is not functional.");
    } else {
      if (size() >= MAX_VARS) {
      }
      // TODO: Throw an exception;
      data_.insert(it, var);

      offsetMap.push_back(0);
      offsetMap.push_back(0);
    }
  }
}

bool VariableFactory::contains(std::string var) {
  return std::binary_search(data_.begin(), data_.end(), var);
}

bool VariableFactory::empty() { return data_.empty(); }

bool VariableFactory::operator==(const VariableFactory &vf) const {
  return data_ == vf.data_;
}

std::string FilterFactory::pprint() {
  std::stringstream ss;

  for (auto &elem : code_map_) {
    ss << elem.first << " -> " << elem.second << "\n";
  }

  return ss.str();
}

int FilterFactory::add_filter(CharClass ccb) {
  auto res = code_map_.insert({ccb, size_});
  if (res.second) {
    filter_map_.push_back(ccb);
    return size_++;
  } else {
    return res.first->second;
  }
}

int FilterFactory::get_code(CharClass cs) { return code_map_.at(cs); }

CharClass &FilterFactory ::get_filter(int code) {
  return filter_map_[code];
}

bool FilterFactory::contains(CharClass &cs) const {
  return code_map_.find(cs) != code_map_.end();
}

void FilterFactory::merge(FilterFactory &rest) {
  /**
   * Merges the FilterFactory with another one inplace. Used at parsing
   * post-processing.
   */

  for (auto &it : rest.filter_map_) {
    if (contains(it))
      throw parsing::BadRegex("Regex is not functional.");

    filter_map_.push_back(it);
    code_map_[it] = size_;
    size_++;
  }
}

std::unordered_map<std::vector<bool>, std::vector<char>>
FilterFactory::allPossibleCharBitsets() {
  std::unordered_map<std::vector<bool>, std::vector<char>> ret;
  for (char a = (char)32; a < 127; a++) {
    std::vector<bool> bs = apply_filters(a);
    auto it = ret.find(bs);
    if (it == ret.end())
      ret.insert(std::make_pair(bs, std::vector<char>(1, a)));
    else
      it->second.push_back(a);
  }
  return ret;
}

std::vector<bool> FilterFactory::apply_filters(char a) {
  /**
   * Returns the bitset vector corresponding to the character after applying
   * all the filters. Searches in bitsetMap hash table. Computes and stores
   * the bitset if not found.
   *
   * Example:
   * Suppose there are 3 filters: "a", "[0-9ab]", "[abc]" with corresponding
   *	codes 0, 1, 2. Then the following table shows the returned bitsets for
   *	different chars
   *
   *	 	CHAR 		 RETURNED BITSET
   *	 		a  					111
   *	 		b 					011
   *	 		c 					001
   *	 		1					010
   *	 		(					000
   */

  auto it = bitsetMap.find(a);

  if (it == bitsetMap.end()) {
    std::vector<bool> bitsetVector(size_);
    for (size_t i = 0; i < size_; i++) {
      bitsetVector[i] = filter_map_[i].contains(a);
    }
    bitsetMap.insert(std::make_pair(a, bitsetVector));

    return bitsetVector;
  }

  return it->second;
}

} // end namespace rematch
