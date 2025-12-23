#include <REmatch/s_query.hpp>

#include <REmatch/s_match.hpp>
#include "evaluation/stream.hpp"
#include "mediator/mediator_constructor.hpp"
#include "mediator/stream_mediator/stream_mediator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

SQuery::SQuery(const std::string& pattern, Flags flags, uint_fast32_t max_mempool_duplications,
               uint_fast32_t max_deterministic_states, uint_fast32_t buffer_size)
    : flags_(flags),
      query_data_(
          std::make_shared<QueryData>(get_query_data(pattern, flags, max_deterministic_states))),
      max_mempool_duplications(max_mempool_duplications),
      max_deterministic_states(max_deterministic_states),
      buffer_size(buffer_size) {}

std::unique_ptr<SMatch> SQuery::find(Reader* reader) {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  auto mediator = MediatorConstructor::create_stream_findone_mediator(*query_data_, stream);

  std::unique_ptr<mediator::Mapping> mapping = mediator->next();

  if (mapping != nullptr) {
    return std::make_unique<SMatch>(std::move(mapping), query_data_->variable_catalog, stream);
  }

  return nullptr;
}

SMatchGenerator SQuery::finditer(Reader* reader) {
  auto stream = std::make_shared<Stream>(reader, buffer_size);
  return {query_data_, stream};
}

}  // namespace REmatch
