#pragma once

#include "evaluation/algorithm/algorithm_class.hpp"
#include "mediator/mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

struct Statistics;

class Mediator {
 public:
  explicit Mediator(QueryData& query_data);

  virtual ~Mediator() = default;
  virtual std::unique_ptr<mediator::Mapping> next() = 0;

 protected:
  int64_t number_of_variables_;
  const Mapping* mapping_ = nullptr;

  std::unique_ptr<mediator::Mapping> construct_user_mapping();

  friend std::unique_ptr<Statistics> collect_statistics(
      const Mediator& mediator);
};
}  // namespace REmatch
