#pragma once

#include <memory>
#include <string>

#include "flags.hpp"
#include "query_data.hpp"
#include "match.hpp"
#include "match_iterator.hpp"

namespace REmatch {

inline namespace library_interface {

class Query {
 private:
  Flags flags_;
  QueryData query_data_;

 public:
  explicit Query(const std::string& pattern, Flags flags = Flags());

  std::unique_ptr<Match> findone(const std::string& text);
  std::unique_ptr<MatchIterator> finditer(const std::string& text);
  bool check(const std::string& text);
  size_t count(const std::string& text);
};

}  // end namespace library_interface
}  // namespace REmatch
