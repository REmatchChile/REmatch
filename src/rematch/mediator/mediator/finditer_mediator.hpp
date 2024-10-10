#ifndef FINDITER_MEDIATOR_HPP
#define FINDITER_MEDIATOR_HPP

#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

class FinditerMediator : public Mediator {
 public:
  FinditerMediator(QueryData& query_data, std::shared_ptr<Document> document,
                   uint_fast32_t max_mempool_duplications);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool next_is_computed_successfully();
};

}  // namespace REmatch

#endif
