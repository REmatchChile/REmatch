#ifndef MATCH_ITERATOR__REMATCH_HPP
#define MATCH_ITERATOR__REMATCH_HPP

#include <string_view>

#include "document_utils.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "library_interface/match.hpp"
#include "match_iterator.hpp"
#include "mediator/mediation_subjects.hpp"
#include "mediator/mediator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "statistics.hpp"
#include "stats_collector.hpp"

namespace REMatch {

inline namespace library_interface {
class MatchIterator {

 private:
  rematch::Mediator mediator_;
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog_;

 public:
  MatchIterator(rematch::MediationSubjects& mediation_subjects,
                std::string&& document, Flags flags = Flags());

  std::unique_ptr<Match> next();
  std::vector<std::string> variables();

  Statistics stats = {};
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
