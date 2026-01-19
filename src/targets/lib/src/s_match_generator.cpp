#include "REmatch/s_match_generator.hpp"

#include "match/stream/s_match.hpp"
#include "mediator/mediator.hpp"
#include "mediator/mediator_constructor.hpp"
#include "parsing/variable_catalog.hpp"

#include <utility>

namespace REmatch {
inline namespace library_interface {

SMatchGenerator::iterator::iterator(std::unique_ptr<Mediator> mediator_,
                                    std::shared_ptr<VariableCatalog> variable_catalog_,
                                    std::shared_ptr<Stream> stream)
    : mediator(std::move(mediator_)),
      variable_catalog(std::move(variable_catalog_)),
      stream(std::move(stream)),
      match_ptr(nullptr) {
  next();
}

SMatchGenerator::iterator::iterator(iterator&& other) noexcept
    : mediator(std::move(other.mediator)),
      variable_catalog(std::move(other.variable_catalog)),
      stream(std::move(other.stream)),
      match_ptr(std::move(other.match_ptr)) {}

SMatchGenerator::iterator& SMatchGenerator::iterator::operator=(iterator&& other) noexcept {
  mediator = std::move(other.mediator);
  variable_catalog = std::move(other.variable_catalog);
  stream = std::move(other.stream);
  match_ptr = std::move(other.match_ptr);
  return *this;
}

SMatchGenerator::iterator::iterator() : match_ptr(nullptr) {}

SMatchGenerator::iterator::~iterator() = default;

SMatchGenerator::iterator::reference SMatchGenerator::iterator::operator*() const {
  return *match_ptr;
}

SMatchGenerator::iterator::pointer SMatchGenerator::iterator::operator->() const {
  return match_ptr.get();
}

SMatchGenerator::iterator& SMatchGenerator::iterator::operator++() {
  next();
  return *this;
}

void SMatchGenerator::iterator::operator++(int) {
  next();
}

bool SMatchGenerator::iterator::operator==(const iterator& other) const {
  return match_ptr == other.match_ptr;
}

bool SMatchGenerator::iterator::operator!=(const iterator& other) const {
  return match_ptr != other.match_ptr;
}

void SMatchGenerator::iterator::next() {
  auto mapping = mediator->next();

  if (mapping) {
    auto match = std::make_unique<SMatch>(std::move(mapping), variable_catalog, stream);
    match_ptr = std::make_unique<MatchTypeErased>(std::move(match));
    return;
  }

  match_ptr = nullptr;
}

// SMatchGenerator

SMatchGenerator::SMatchGenerator(std::shared_ptr<QueryData> query_data,
                                 std::shared_ptr<Stream> stream)
    : query_data(std::move(query_data)), stream(std::move(stream)) {}

SMatchGenerator::iterator SMatchGenerator::begin() const {
  std::unique_ptr<Mediator> mediator =
      MediatorConstructor::create_stream_mediator(*query_data, stream);

  return iterator(std::move(mediator), query_data->variable_catalog, stream);
}

SMatchGenerator::iterator SMatchGenerator::end() const {
  return {};
}

}  // namespace library_interface

}  // namespace REmatch
