#include <REmatch/match_iterator.hpp>

#include "evaluation/document.hpp"
#include "mediator/mediator/finditer_mediator.hpp"
#include "mediator/mediator/mediator.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/query_data.hpp"
#include "utils/statistics.hpp"

namespace REmatch {
inline namespace library_interface {
MatchIterator::MatchIterator(QueryData& query_data, const std::string& str,
                             uint_fast32_t max_mempool_duplications, uint_fast32_t max_deterministic_states)
    : variable_catalog_(query_data.variable_catalog),
      document_(std::make_shared<Document>(str)) {
  mediator_ = std::make_unique<FinditerMediator>(query_data, document_,
                                                 max_mempool_duplications, max_deterministic_states);
}

MatchIterator::MatchIterator(const std::string& pattern, const std::string& str,
                             Flags flags,
                             uint_fast32_t max_mempool_duplications,
                             uint_fast32_t max_deterministic_states)
    : query_data_(get_query_data(pattern, flags, max_deterministic_states)),
      document_(std::make_shared<Document>(str)) {

  variable_catalog_ = query_data_->variable_catalog;
  mediator_ = std::make_unique<FinditerMediator>(*query_data_, document_,
                                                 max_mempool_duplications,
                                                 max_deterministic_states);
}

MatchIterator::~MatchIterator() = default;

std::unique_ptr<Match> MatchIterator::next() {
  auto mapping = mediator_->next();

  if (mapping != nullptr) {
    return std::make_unique<Match>(std::move(mapping), variable_catalog_, document_);
  }

  stats = collect_statistics(*mediator_);

  return nullptr;
}

}  // end namespace library_interface
}  // namespace REmatch
