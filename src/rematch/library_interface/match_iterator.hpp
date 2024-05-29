#ifndef MATCH_ITERATOR__REMATCH_HPP
#define MATCH_ITERATOR__REMATCH_HPP

#include <string_view>

#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "library_interface/match.hpp"
#include "mediator/mediator/mediator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "query_data/query_data_utils.hpp"
#include "statistics.hpp"
#include "stats_collector.hpp"

namespace rematch {
class Document;
}

namespace REMatch {

inline namespace library_interface {
class MatchIterator {

 private:
  std::optional<rematch::QueryData> query_data_ = std::nullopt;
  std::unique_ptr<rematch::Mediator> mediator_;
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<rematch::Document> document_;

 public:
  MatchIterator(rematch::QueryData& query_data,
                std::string_view str,
                Flags flags = Flags());
  MatchIterator(const std::string& pattern,
                std::string_view str,
                Flags flags = Flags());

  std::unique_ptr<Match> next();
  std::vector<std::string> variables();

  Statistics stats = {};
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
