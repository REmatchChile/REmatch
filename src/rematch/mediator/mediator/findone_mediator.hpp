#ifndef FINDONE_MEDIATOR_HPP
#define FINDONE_MEDIATOR_HPP

#include <REmatch/query_data.hpp>

#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"

namespace REmatch {

class FindoneMediator : public Mediator {
 public:
  FindoneMediator(QueryData& query_data, std::shared_ptr<Document> document,
                  uint_fast32_t max_mempool_duplications);

  mediator::Mapping* next() override;

 private:
  bool has_output = false;
};

}  // namespace REmatch

#endif
