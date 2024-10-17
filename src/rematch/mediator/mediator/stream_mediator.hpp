#pragma once

#include <string_view>
#include "evaluation/stream.hpp"
#include "filtering_module/segment_identificator_stream.hpp"
#include "library_interface/reader.hpp"
#include "mediator/mapping.hpp"
#include "mediator/mediator/mediator.hpp"

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/algorithm/stream_algorithm.hpp"
#include "mediator.hpp"

#include "evaluation/document.hpp"

namespace rematch {

class StreamMediator {
 public:
  StreamMediator(QueryData& query_data, std::shared_ptr<Stream> stream,
                 Flags flags)
      : variable_catalog_(query_data.variable_catalog) {
    algorithm_ = std::make_unique<StreamAlgorithm>(query_data.extended_va,
                                                   stream, flags);
  }

  mediator::Mapping* next() {
    mapping_ = algorithm_->get_next_mapping();

    if (mapping_ != nullptr) {
      return construct_user_mapping();
    }

    return nullptr;
  }

  mediator::Mapping* construct_user_mapping() {
    static mediator::Mapping result_mapping;
    result_mapping.reset();

    std::map<int, std::vector<Span>> spans_map = mapping_->construct_mapping();

    for (unsigned int variable_id = 0; variable_id < variable_catalog_->size();
         variable_id++) {
      Span span = spans_map[static_cast<int>(variable_id)].back();

      std::string variable_name = variable_catalog_->get_var(variable_id);
      result_mapping.add_span(variable_name, span);
    }

    // -1 because of the START_CHAR that was added to the document
    result_mapping.shift_spans(-1);
    return &result_mapping;
  }

  std::shared_ptr<VariableCatalog> variable_catalog_ = nullptr;
  std::unique_ptr<StreamAlgorithm> algorithm_ = nullptr;

  const Mapping* mapping_{};
};

}  // namespace rematch
