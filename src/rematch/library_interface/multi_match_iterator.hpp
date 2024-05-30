#ifndef MULTI_MATCH_ITERATOR_REMATCH_HPP
#define MULTI_MATCH_ITERATOR_REMATCH_HPP

#include <memory>
#include "evaluation/document.hpp"
#include "library_interface/multi_match.hpp"
#include "mediator/mediator/multi_finditer_mediator.hpp"
#include "parsing/variable_catalog.hpp"

namespace REMatch {

inline namespace library_interface {
class MultiMatchIterator {

 private:
std::optional<rematch::QueryData> query_data_ = std::nullopt;
  std::unique_ptr<MultiFinditerMediator> mediator_;
  std::shared_ptr<rematch::parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<rematch::Document> document_;

 public:
  MultiMatchIterator(rematch::QueryData& query_data, std::string_view str,
                     Flags flags = Flags());

  std::unique_ptr<MultiMatch> next();

  std::vector<std::string> variables();
};

}  // end namespace library_interface
}  // namespace REMatch

#endif
