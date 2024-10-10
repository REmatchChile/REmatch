#ifndef FINDONE_MEDIATOR_HPP
#define FINDONE_MEDIATOR_HPP

#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

class FindoneMediator : public Mediator {
 public:
  FindoneMediator(QueryData& query_data, std::shared_ptr<Document> document,
                  uint_fast32_t max_mempool_duplications);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool has_output = false;
};

}  // namespace REmatch

#endif
