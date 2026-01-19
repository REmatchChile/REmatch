#include "match_standard_generator.hpp"

#include "match/match/match_standard.hpp"
#include "mediator/finditer_mediator/finditer_mediator.hpp"
#include "mediator/mediator_constructor.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/statistics.hpp"

namespace REmatch::internal {

/* MatchGenerator iterator */

MatchStandardGenerator::iterator::iterator(std::unique_ptr<Mediator> mediator_,
                                           std::shared_ptr<VariableCatalog> variable_catalog_,
                                           std::shared_ptr<Document> document)
    : mediator(std::move(mediator_)),
      variable_catalog(std::move(variable_catalog_)),
      document(std::move(document)),
      match_ptr(nullptr) {
  next();
}

MatchStandardGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      document(std::move(other.document)),
      stats(std::move(other.stats)),
      match_ptr(std::move(other.match_ptr)) {}

MatchStandardGenerator::iterator& MatchStandardGenerator::iterator::operator=(
    iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  document = std::move(other.document);
  stats = std::move(other.stats);
  match_ptr = std::move(other.match_ptr);
  return *this;
}

MatchStandardGenerator::iterator::iterator() : match_ptr(nullptr) {}

MatchStandardGenerator::iterator::~iterator() = default;

MatchStandardGenerator::iterator::reference MatchStandardGenerator::iterator::operator*() const {
  return *match_ptr;
}

MatchStandardGenerator::iterator::pointer MatchStandardGenerator::iterator::operator->() const {
  return match_ptr.get();
}

MatchStandardGenerator::iterator& MatchStandardGenerator::iterator::operator++() {
  next();
  return *this;
}

void MatchStandardGenerator::iterator::operator++(int) {
  next();
}

bool MatchStandardGenerator::iterator::operator==(const iterator& other) const {
  return match_ptr == other.match_ptr;
}

bool MatchStandardGenerator::iterator::operator!=(const iterator& other) const {
  return match_ptr != other.match_ptr;
}

void MatchStandardGenerator::iterator::next() {
  auto mapping = mediator->next();

  if (mapping) {
    auto match =
        std::make_unique<internal::MatchStandard>(std::move(mapping), variable_catalog, document);
    match_ptr = std::make_unique<REmatch::Match>(std::move(match));
    return;
  }

  match_ptr = nullptr;
  stats = collect_statistics(*mediator);
}

/* MatchGenerator */

MatchStandardGenerator::MatchStandardGenerator(std::shared_ptr<QueryData> query_data,
                                               std::shared_ptr<Document> document)
    : query_data(std::move(query_data)), document(std::move(document)) {}

MatchStandardGenerator::iterator MatchStandardGenerator::begin() const {
  std::unique_ptr<Mediator> mediator =
      MediatorConstructor::create_finditer_mediator(*query_data, document);
  return iterator(std::move(mediator), query_data->variable_catalog, document);
}

MatchStandardGenerator::iterator MatchStandardGenerator::end() const {
  return {};
}

}  // namespace REmatch::internal
