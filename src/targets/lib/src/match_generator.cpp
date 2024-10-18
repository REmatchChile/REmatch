#include <REmatch/match_generator.hpp>

#include "mediator/mediator/finditer_mediator.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/statistics.hpp"

#include <REmatch/match.hpp>

namespace REmatch {
inline namespace library_interface {

MatchGenerator::iterator::iterator(
    std::unique_ptr<FinditerMediator> mediator_,
    std::shared_ptr<VariableCatalog> variable_catalog_,
    std::shared_ptr<Document> document_)
    : mediator(std::move(mediator_)),
      variable_catalog(variable_catalog_),
      document(document_),
      match_ptr(nullptr) {
  next();
}

MatchGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      document(std::move(other.document)),
      stats(std::move(other.stats)),
      match_ptr(std::move(other.match_ptr)) {}

MatchGenerator::iterator& MatchGenerator::iterator::operator=(
    iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  document = std::move(other.document);
  stats = std::move(other.stats);
  match_ptr = std::move(other.match_ptr);
  return *this;
}

MatchGenerator::iterator::iterator() : match_ptr(nullptr) {}

MatchGenerator::iterator::~iterator() = default;

MatchGenerator::iterator::reference MatchGenerator::iterator::operator*()
    const {
  return *match_ptr;
}

MatchGenerator::iterator::pointer MatchGenerator::iterator::operator->() const {
  return match_ptr.get();
}

MatchGenerator::iterator& MatchGenerator::iterator::operator++() {
  next();
  return *this;
}

void MatchGenerator::iterator::operator++(int) {
  next();
}

bool MatchGenerator::iterator::operator==(const iterator& other) const {
  return match_ptr == other.match_ptr;
}

bool MatchGenerator::iterator::operator!=(const iterator& other) const {
  return match_ptr != other.match_ptr;
}

void MatchGenerator::iterator::next() {
  auto mapping = mediator->next();

  if (mapping) {
    match_ptr =
        std::make_unique<Match>(std::move(mapping), variable_catalog, document);
    return;
  }

  match_ptr = nullptr;
  stats = collect_statistics(*mediator);
}

MatchGenerator::MatchGenerator(std::shared_ptr<QueryData> query_data,
                               const std::string& document,
                               uint_fast32_t max_mempool_duplications,
                               uint_fast32_t max_deterministic_states)
    : query_data(query_data),
      document(std::make_shared<Document>(document)),
      max_mempool_duplications(max_mempool_duplications),
      max_deterministic_states(max_deterministic_states) {}

MatchGenerator::iterator MatchGenerator::begin() const {
  auto mediator = std::make_unique<FinditerMediator>(*query_data, document,
                                                     max_mempool_duplications,
                                                     max_deterministic_states);

  return iterator(std::move(mediator), query_data->variable_catalog, document);
}

MatchGenerator::iterator MatchGenerator::end() const {
  return {};
}

}  // namespace library_interface

}  // namespace REmatch
