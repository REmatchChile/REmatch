#include <REmatch/match_generator.hpp>

#include "mediator/finditer_mediator/finditer_mediator.hpp"
#include "mediator/mediator_constructor.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/statistics.hpp"

#include <REmatch/match.hpp>

namespace REmatch {
inline namespace library_interface {

/* MatchGenerator iterator */

MatchGenerator::iterator::iterator(std::unique_ptr<Mediator> mediator_,
                                   std::shared_ptr<VariableCatalog> variable_catalog_,
                                   std::shared_ptr<Document> document)
    : mediator(std::move(mediator_)),
      variable_catalog(std::move(variable_catalog_)),
      document(std::move(document)),
      match_ptr(nullptr) {
  next();
}

MatchGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      document(std::move(other.document)),
      stats(std::move(other.stats)),
      match_ptr(std::move(other.match_ptr)) {}

MatchGenerator::iterator& MatchGenerator::iterator::operator=(iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  document = std::move(other.document);
  stats = std::move(other.stats);
  match_ptr = std::move(other.match_ptr);
  return *this;
}

MatchGenerator::iterator::iterator() : match_ptr(nullptr) {}

MatchGenerator::iterator::~iterator() = default;

MatchGenerator::iterator::value MatchGenerator::iterator::operator*() {
  return std::move(match_ptr);
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
    match_ptr = std::make_unique<MatchStandard>(std::move(mapping), variable_catalog, document);
    return;
  }

  match_ptr = nullptr;
  stats = collect_statistics(*mediator);
}

/* MatchGenerator */

MatchGenerator::MatchGenerator(std::shared_ptr<QueryData> query_data,
                               std::shared_ptr<Document> document)
    : query_data(std::move(query_data)), document(std::move(document)) {}

MatchGenerator::iterator MatchGenerator::begin() const {
  std::unique_ptr<Mediator> mediator =
      MediatorConstructor::create_finditer_mediator(*query_data, document);
  return iterator(std::move(mediator), query_data->variable_catalog, document);
}

MatchGenerator::iterator MatchGenerator::end() const {
  return {};
}

}  // namespace library_interface

}  // namespace REmatch
