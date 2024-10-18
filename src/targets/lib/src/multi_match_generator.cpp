#include <REmatch/multi_match_generator.hpp>

#include "mediator/mediator/multi_finditer_mediator.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/statistics.hpp"

#include <REmatch/multi_match.hpp>

namespace REmatch {
inline namespace library_interface {

MultiMatchGenerator::iterator::iterator(
    std::unique_ptr<MultiFinditerMediator> mediator_,
    std::shared_ptr<VariableCatalog> variable_catalog_,
    std::shared_ptr<Document> document_)
    : mediator(std::move(mediator_)),
      variable_catalog(variable_catalog_),
      document(document_),
      multi_match_ptr(nullptr) {
  next();
}

MultiMatchGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      document(std::move(other.document)),
      multi_match_ptr(std::move(other.multi_match_ptr)) {}

MultiMatchGenerator::iterator& MultiMatchGenerator::iterator::operator=(
    iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  document = std::move(other.document);
  multi_match_ptr = std::move(other.multi_match_ptr);
  return *this;
}

MultiMatchGenerator::iterator::iterator() : multi_match_ptr(nullptr) {}

MultiMatchGenerator::iterator::~iterator() = default;

MultiMatchGenerator::iterator::reference
MultiMatchGenerator::iterator::operator*() const {
  return *multi_match_ptr;
}

MultiMatchGenerator::iterator::pointer
MultiMatchGenerator::iterator::operator->() const {
  return multi_match_ptr.get();
}

MultiMatchGenerator::iterator& MultiMatchGenerator::iterator::operator++() {
  next();
  return *this;
}

void MultiMatchGenerator::iterator::operator++(int) {
  next();
}

bool MultiMatchGenerator::iterator::operator==(const iterator& other) const {
  return multi_match_ptr == other.multi_match_ptr;
}

bool MultiMatchGenerator::iterator::operator!=(const iterator& other) const {
  return multi_match_ptr != other.multi_match_ptr;
}

void MultiMatchGenerator::iterator::next() {
  auto mapping = mediator->next();

  if (mapping) {
    multi_match_ptr = std::make_unique<MultiMatch>(std::move(mapping),
                                                   variable_catalog, document);
    return;
  }

  multi_match_ptr = nullptr;
}

MultiMatchGenerator::MultiMatchGenerator(std::shared_ptr<QueryData> query_data,
                                         const std::string& document,
                                         uint_fast32_t max_mempool_duplications,
                                         uint_fast32_t max_deterministic_states)
    : query_data(query_data),
      document(std::make_shared<Document>(document)),
      max_mempool_duplications(max_mempool_duplications),
      max_deterministic_states(max_deterministic_states) {}

MultiMatchGenerator::iterator MultiMatchGenerator::begin() const {
  auto mediator = std::make_unique<MultiFinditerMediator>(
      *query_data, document, max_mempool_duplications,
      max_deterministic_states);

  return iterator(std::move(mediator), query_data->variable_catalog, document);
}

MultiMatchGenerator::iterator MultiMatchGenerator::end() const {
  return {};
}

}  // namespace library_interface

}  // namespace REmatch
