#ifndef MULTI_QUERY_HPP
#define MULTI_QUERY_HPP

#include "evaluation/document.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/query_data/query_data.hpp"
#include "multi_match_iterator.hpp"

namespace REMatch {
using namespace rematch;

class MultiQuery {
 private:
  Flags flags_;
  QueryData query_data_;

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags = Flags());
  std::unique_ptr<MultiMatch> findone(const std::string& text);
  std::unique_ptr<REMatch::MultiMatchIterator> finditer(const std::string& text);
  bool check(const std::string& text);
};

}  // namespace REMatch

#endif
