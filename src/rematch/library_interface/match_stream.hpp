#pragma once

#include <memory>
#include <string>
#include <utility>

#include "mediator/mapping.hpp"
#include "output_enumeration/span.hpp"
#include "parsing/variable_catalog.hpp"

using namespace rematch;

class MatchStream {
 public:
  MatchStream(mediator::Mapping mapping,
              std::shared_ptr<VariableCatalog> variable_catalog)
      : mapping_(std::move(mapping)),
        variable_catalog_(std::move(variable_catalog)) {}

  Span span(std::string variable_name) {
    return mapping_.get_span_of_variable(std::move(variable_name));
  }

  Span span(int variable_id) {
    return span(variable_catalog_->get_var(variable_id));
  }

  mediator::Mapping mapping_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
};