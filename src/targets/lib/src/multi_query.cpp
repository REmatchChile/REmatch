#include <REmatch/multi_query.hpp>

#include <REmatch/flags.hpp>
#include <REmatch/query_data.hpp>
#include <cstdint>

#include "evaluation/document.hpp"
#include "mediator/mediator/multi_findone_mediator.hpp"
#include "mediator/output_checker.hpp"

namespace REmatch {

MultiQuery::MultiQuery(const std::string& pattern, Flags flags,
                       uint_fast32_t max_mempool_duplications,
                       uint_fast32_t max_deterministic_states)
    : query_data_(
          get_multi_query_data(pattern, flags, max_deterministic_states)),
      max_mempool_duplications_(max_mempool_duplications) {}

std::unique_ptr<MultiMatch> MultiQuery::findone(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);

  auto mediator =
      MultiFindoneMediator(query_data_, document, max_mempool_duplications_);

  std::unique_ptr<ExtendedMapping> mapping = mediator.next();

  if (mapping != nullptr) {
    return std::make_unique<MultiMatch>(*mapping, query_data_.variable_catalog,
                                        document);
  }
  return nullptr;
}

std::vector<MultiMatch> MultiQuery::findall(const std::string& text) {
  std::vector<MultiMatch> res;

  auto match_iterator = finditer(text);

  while (auto match = match_iterator->next()) {
    res.push_back(*match);
  }

  return res;
}

std::unique_ptr<MultiMatchIterator> MultiQuery::finditer(
    const std::string& text) {
  return std::make_unique<MultiMatchIterator>(query_data_, text,
                                              max_mempool_duplications_);
}

bool MultiQuery::check(const std::string& text) {
  std::shared_ptr<Document> document = std::make_shared<Document>(text);
  auto output_checker = OutputChecker(query_data_, document);
  return output_checker.check();
}

}  // namespace REmatch
