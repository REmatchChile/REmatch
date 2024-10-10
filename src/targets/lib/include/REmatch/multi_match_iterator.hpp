#pragma once

#include <memory>

#include "multi_match.hpp"

namespace REmatch {
class Document;
class MultiFinditerMediator;
struct QueryData;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {
class MultiMatch;

class MultiMatchIterator {

 public:
  MultiMatchIterator(QueryData& query_data, const std::string& str,
                     uint_fast32_t max_mempool_duplications,
                     uint_fast32_t max_deterministic_states);

  ~MultiMatchIterator();

  std::unique_ptr<MultiMatch> next();

  std::vector<std::string> variables();

 private:
  std::unique_ptr<MultiFinditerMediator> mediator_;
  std::shared_ptr<parsing::VariableCatalog> variable_catalog_;
  std::shared_ptr<Document> document_;
};

}  // end namespace library_interface
}  // namespace REmatch
