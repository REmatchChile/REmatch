#pragma once

#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"
#include "utils/query_data.hpp"

#include <REmatch/constants.hpp>

namespace REmatch {

class FindoneMediator : public Mediator {
 public:
  FindoneMediator(QueryData& query_data, std::shared_ptr<Document> document,
                  uint_fast32_t max_mempool_duplications =
                      REmatch::DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                  uint_fast32_t max_deterministic_states =
                      REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  std::unique_ptr<mediator::Mapping> next() override;

 private:
  bool has_output = false;
};

}  // namespace REmatch
