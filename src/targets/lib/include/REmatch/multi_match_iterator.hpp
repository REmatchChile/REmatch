#pragma once

#include <memory>

#include "evaluation/document.hpp"
#include "flags.hpp"
#include "mediator/mediator/multi_finditer_mediator.hpp"
#include "multi_match.hpp"
#include "parsing/variable_catalog.hpp"
#include "query_data.hpp"

namespace REmatch {

inline namespace library_interface {
class MultiMatch;
class MultiMatchIterator {

 private:
  std::optional<QueryData> query_data_ = std::nullopt;
  std::unique_ptr<MultiFinditerMediator> mediator_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;

 public:
  MultiMatchIterator(QueryData& query_data, const std::string& str,
                     Flags flags = Flags());

  std::unique_ptr<MultiMatch> next();

  std::vector<std::string> variables();
};

}  // end namespace library_interface
}  // namespace REmatch
