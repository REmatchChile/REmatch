#ifndef MULTI_REGEX_HPP
#define MULTI_REGEX_HPP

#include "evaluation/document.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/regex_data/regex_data.hpp"
#include "multi_match_iterator.hpp"

namespace REMatch {
using namespace rematch;

class MultiRegex {
 private:
  Flags flags_;
  RegexData regex_data_;

 public:
  MultiRegex(std::string_view pattern, Flags flags = Flags());
  std::unique_ptr<MultiMatch> findone(std::string_view text);
  std::unique_ptr<REMatch::MultiMatchIterator> finditer(std::string_view text);
  bool check(std::string_view text);
};

}  // namespace REMatch

#endif
