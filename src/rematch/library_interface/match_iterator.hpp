#ifndef MATCH_ITERATOR__REMATCH_HPP
#define MATCH_ITERATOR__REMATCH_HPP

#include <string_view>

#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "library_interface/match.hpp"
#include "mediator/mediator.hpp"

namespace REMatch {

inline namespace library_interface {
class MatchIterator {

 private:
  rematch::Mediator mediator_;
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog_;
  std::string_view& document_;

 public:
  MatchIterator(rematch::Mediator&& mediator,
      std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog,
      std::string_view& document);
  std::unique_ptr<Match> next();
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
