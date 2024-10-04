#ifndef FINDITER_MEDIATOR_HPP
#define FINDITER_MEDIATOR_HPP

#include <REmatch/query_data.hpp>
#include <string_view>
#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"

namespace REmatch {

class FinditerMediator : public Mediator {
 public:
  FinditerMediator(QueryData& query_data, std::shared_ptr<Document> document,
                   Flags flags = Flags());

  mediator::Mapping* next() override;

 private:
  bool next_is_computed_successfully();
};

}  // namespace REmatch

#endif
