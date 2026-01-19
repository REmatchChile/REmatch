#include "multi_match_generator.hpp"

#include <utility>

#include "REmatch/multi_match.hpp"
#include "match/match/multi_match_standard.hpp"
#include "mediator/mediator_constructor.hpp"
#include "parsing/variable_catalog.hpp"
#include "utils/statistics.hpp"

namespace REmatch::internal {

MultiMatchGenerator::iterator::iterator(std::unique_ptr<MultiMediator> mediator_,
                                        std::shared_ptr<VariableCatalog> variable_catalog_,
                                        std::shared_ptr<Document> document_)
    : mediator(std::move(mediator_)),
      variable_catalog(std::move(variable_catalog_)),
      document(std::move(document_)),
      multi_match_ptr(nullptr) {
  next();
}

MultiMatchGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      document(std::move(other.document)),
      multi_match_ptr(std::move(other.multi_match_ptr)) {}

MultiMatchGenerator::iterator& MultiMatchGenerator::iterator::operator=(iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  document = std::move(other.document);
  multi_match_ptr = std::move(other.multi_match_ptr);
  return *this;
}

MultiMatchGenerator::iterator::iterator() : multi_match_ptr(nullptr) {}

MultiMatchGenerator::iterator::~iterator() = default;

MultiMatchGenerator::iterator::reference MultiMatchGenerator::iterator::operator*() const {
  return *multi_match_ptr;
}

MultiMatchGenerator::iterator::pointer MultiMatchGenerator::iterator::operator->() const {
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
    auto match =
        std::make_unique<internal::MultiMatchStandard>(std::move(mapping), variable_catalog, document);
    multi_match_ptr = std::make_unique<REmatch::MultiMatch>(std::move(match));
    return;
  }

  multi_match_ptr = nullptr;
}

MultiMatchGenerator::MultiMatchGenerator(std::shared_ptr<QueryData> query_data,
                                         std::shared_ptr<Document> document)
    : query_data(std::move(query_data)), document(std::move(document)) {}

MultiMatchGenerator::iterator MultiMatchGenerator::begin() const {
  auto mediator =
      MediatorConstructor::create_multi_finditer_mediator(*this->query_data, this->document);

  return iterator(std::move(mediator), query_data->variable_catalog, document);
}

MultiMatchGenerator::iterator MultiMatchGenerator::end() const {
  return {};
}

}  // namespace REmatch::internal
