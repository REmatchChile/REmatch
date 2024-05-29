#ifndef LIBRARY_INTERFACE__QUERY_HPP
#define LIBRARY_INTERFACE__QUERY_HPP

#include <optional>
#include <string>
#include <vector>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "evaluation/start_end_chars.hpp"
#include "statistics.hpp"
#include "query_data/query_data_utils.hpp"

namespace REMatch {
inline namespace library_interface {

class Query {
 private:
  Flags flags_;
  rematch::QueryData query_data_;

 public:
  explicit Query(std::string_view pattern, Flags flags = Flags());

  std::unique_ptr<Match> findone(std::string_view text);
  std::unique_ptr<MatchIterator> finditer(std::string_view text);
  bool check(std::string_view text);
  size_t count(std::string_view text);
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
