#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>

#include "constants.hpp"

#include "REmatch_export.hpp"

namespace REmatch {
class Document;
class FinditerMediator;
struct QueryData;
struct Statistics;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {
class Match;

class REMATCH_EXPORT MatchGenerator {
 public:
  struct REMATCH_EXPORT iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value = Match;
    using pointer = Match*;
    using reference = Match&;

    // called with begin()
    explicit iterator(std::unique_ptr<FinditerMediator> mediator_,
                      std::shared_ptr<VariableCatalog> variable_catalog_,
                      std::shared_ptr<Document> document_);

    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;

    // called with end()
    iterator();

    ~iterator();

    reference operator*() const;
    pointer operator->() const;

    iterator& operator++();
    void operator++(int);

    bool operator==(const iterator& other) const;

    bool operator!=(const iterator& other) const;

   private:
    std::unique_ptr<FinditerMediator> mediator;

    std::shared_ptr<parsing::VariableCatalog> variable_catalog;

    std::shared_ptr<Document> document;

    std::unique_ptr<Statistics> stats;

    std::unique_ptr<value> match_ptr;

    void next();
  };

  MatchGenerator(
      std::shared_ptr<QueryData> query_data, const std::string& document,
      uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
      uint_fast32_t max_deterministic_states =
          DEFAULT_MAX_DETERMINISTIC_STATES);

  iterator begin() const;

  iterator end() const;

 private:
  std::shared_ptr<QueryData> query_data;

  std::shared_ptr<Document> document;

  uint_fast32_t max_mempool_duplications;
  uint_fast32_t max_deterministic_states;
};

}  // namespace library_interface
}  // namespace REmatch