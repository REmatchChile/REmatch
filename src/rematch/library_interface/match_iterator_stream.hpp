#pragma once

#include "library_interface/query_data/query_data.hpp"
#include "library_interface/reader.hpp"
#include "library_interface/match_stream.hpp"

#include "mediator/mediator/stream_mediator.hpp"

using namespace rematch;

namespace REMatch {

class MatchIteratorStream {
 public:
  MatchIteratorStream(QueryData& query_data, std::shared_ptr<Reader> reader,
                Flags flags)
      : variable_catalog_(query_data.variable_catalog) {
    mediator_ = std::make_unique<rematch::StreamMediator>(
        query_data, std::make_shared<Stream>(reader), flags);
  }

  std::unique_ptr<MatchStream> next() {
    rematch::mediator::Mapping* mapping = mediator_->next();

    if (mapping != nullptr) {
      return std::make_unique<MatchStream>(*mapping, variable_catalog_);
    }

    return nullptr;
  }

  std::vector<std::string> variables() {
    return variable_catalog_->variables();
  }

  std::unique_ptr<StreamMediator> mediator_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
};

}  // namespace REMatch
