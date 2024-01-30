#ifndef MATCH_ITERATOR__REMATCH_HPP
#define MATCH_ITERATOR__REMATCH_HPP

#include <string_view>

#include "document_utils.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "library_interface/match.hpp"
#include "match_iterator.hpp"
#include "mediator/mediator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "statistics.hpp"
#include "stats_collector.hpp"
#include "regex_data/regex_data_utils.hpp"

namespace REMatch {

inline namespace library_interface {
class MatchIterator {

 private:
  std::optional<rematch::RegexData> regex_data_ = std::nullopt;
  std::unique_ptr<rematch::Mediator> mediator_;
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog_;
  const std::string document_;

 public:
  MatchIterator(rematch::RegexData& regex_data, std::string&& document,
                Flags flags = Flags());
  MatchIterator(const std::string& pattern, std::string&& document,
                Flags flags = Flags());

  std::unique_ptr<Match> next();
  std::vector<std::string> variables();

  Statistics stats = {};
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
