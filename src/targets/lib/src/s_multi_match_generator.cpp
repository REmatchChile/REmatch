#include "REmatch/s_multi_match_generator.hpp"

#include "match/stream/s_multi_match.hpp"
#include "mediator/mediator.hpp"
#include "mediator/mediator_constructor.hpp"
#include "parsing/variable_catalog.hpp"

#include <utility>

namespace REmatch {
inline namespace library_interface {

SMultiMatchGenerator::iterator::iterator(std::unique_ptr<MultiMediator> mediator_,
                                         std::shared_ptr<VariableCatalog> variable_catalog_,
                                         std::shared_ptr<Stream> stream)
    : mediator(std::move(mediator_)),
      variable_catalog(std::move(variable_catalog_)),
      stream(std::move(stream)),
      match_ptr(nullptr) {
  next();
}

SMultiMatchGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      match_ptr(std::move(other.match_ptr)) {}

SMultiMatchGenerator::iterator& SMultiMatchGenerator::iterator::operator=(
    iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  match_ptr = std::move(other.match_ptr);
  return *this;
}

SMultiMatchGenerator::iterator::iterator() : match_ptr(nullptr) {}

SMultiMatchGenerator::iterator::~iterator() = default;

SMultiMatchGenerator::iterator::reference SMultiMatchGenerator::iterator::operator*() const {
  return *match_ptr;
}

SMultiMatchGenerator::iterator::pointer SMultiMatchGenerator::iterator::operator->() const {
  return match_ptr.get();
}

SMultiMatchGenerator::iterator& SMultiMatchGenerator::iterator::operator++() {
  next();
  return *this;
}

void SMultiMatchGenerator::iterator::operator++(int) {
  next();
}

bool SMultiMatchGenerator::iterator::operator==(const iterator& other) const {
  return match_ptr == other.match_ptr;
}

bool SMultiMatchGenerator::iterator::operator!=(const iterator& other) const {
  return match_ptr != other.match_ptr;
}

void SMultiMatchGenerator::iterator::next() {
  auto mapping = mediator->next();

  if (mapping) {
    auto match = std::make_unique<SMultiMatch>(std::move(mapping), variable_catalog, stream);
    match_ptr = std::make_unique<MultiMatchTypeErased>(std::move(match));
    return;
  }

  match_ptr = nullptr;
}

// SMultiMatchGenerator

SMultiMatchGenerator::SMultiMatchGenerator(std::shared_ptr<QueryData> query_data,
                                           std::shared_ptr<Stream> stream)
    : query_data(std::move(query_data)), stream(std::move(stream)) {}

SMultiMatchGenerator::iterator SMultiMatchGenerator::begin() const {
  std::unique_ptr<MultiMediator> mediator =
      MediatorConstructor::create_stream_multi_mediator(*query_data, stream);

  return iterator(std::move(mediator), query_data->variable_catalog, stream);
}

SMultiMatchGenerator::iterator SMultiMatchGenerator::end() const {
  return {};
}

}  // namespace library_interface

}  // namespace REmatch
