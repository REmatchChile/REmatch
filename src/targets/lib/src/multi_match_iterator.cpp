#include <REmatch/multi_match_iterator.hpp>

#include <REmatch/multi_match.hpp>

#include <memory>

#include "evaluation/document.hpp"
#include "mediator/mediator/multi_finditer_mediator.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch {

inline namespace library_interface {

MultiMatchIterator::MultiMatchIterator(QueryData& query_data,
                                       const std::string& str, Flags flags)
    : variable_catalog_(query_data.variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ =
      std::make_unique<MultiFinditerMediator>(query_data, document_, flags);
}

std::unique_ptr<MultiMatch> MultiMatchIterator::next() {
  std::unique_ptr<ExtendedMapping> extended_mapping = mediator_->next();

  if (extended_mapping != nullptr) {
    return std::make_unique<MultiMatch>(*extended_mapping, variable_catalog_,
                                        document_);
  }

  return nullptr;
}

std::vector<std::string> MultiMatchIterator::variables() {
  return variable_catalog_->variables();
}

}  // end namespace library_interface
}  // namespace REmatch
