#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <iterator>
#include <memory>

#include "constants.hpp"

#include "REmatch_export.hpp"

namespace REmatch {
class Document;
class MultiFinditerMediator;
struct QueryData;
struct Statistics;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {
class MultiMatch;

class REMATCH_EXPORT MultiMatchGenerator {
 public:
  struct REMATCH_EXPORT iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value = MultiMatch;
    using pointer = MultiMatch*;
    using reference = MultiMatch&;

    // called with begin()
    explicit iterator(std::unique_ptr<MultiFinditerMediator> mediator_,
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
    std::unique_ptr<MultiFinditerMediator> mediator;

    std::shared_ptr<parsing::VariableCatalog> variable_catalog;

    std::shared_ptr<Document> document;

    std::unique_ptr<Statistics> stats;

    std::unique_ptr<value> multi_match_ptr;

    void next();
  };

  MultiMatchGenerator(
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