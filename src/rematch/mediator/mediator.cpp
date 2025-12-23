#include "mediator.hpp"

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/document.hpp"
#include "mediator/mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

Mediator::Mediator(QueryData& query_data)
    : number_of_variables_(query_data.variable_catalog->size()) {}

std::unique_ptr<mediator::Mapping> Mediator::construct_user_mapping() {
  auto res = std::make_unique<mediator::Mapping>();
  std::map<int, std::vector<Span>> spans_map = mapping_->construct_mapping();

  for (int variable_id = 0; variable_id < number_of_variables_; variable_id++) {
    Span span = spans_map[variable_id].back();

    res->add_span(variable_id, span);
  }

  return res;
}

}  // namespace REmatch
