#pragma once

#include "evaluation/document.hpp"
#include "flags.hpp"
#include "multi_match.hpp"
#include "multi_match_iterator.hpp"
#include "query_data.hpp"

namespace REmatch {
inline namespace library_interface {

class MultiQuery {
 private:
  Flags flags_;
  QueryData query_data_;

 public:
  explicit MultiQuery(const std::string& pattern, Flags flags = Flags());
  std::unique_ptr<MultiMatch> findone(const std::string& text);
  std::unique_ptr<MultiMatchIterator> finditer(const std::string& text);
  bool check(const std::string& text);
};
}  // namespace library_interface
}  // namespace REmatch
