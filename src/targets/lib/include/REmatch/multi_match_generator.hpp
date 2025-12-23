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
class MultiMediator;
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
    explicit iterator(std::unique_ptr<MultiMediator> mediator_,
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
    std::unique_ptr<MultiMediator> mediator;

    std::shared_ptr<parsing::VariableCatalog> variable_catalog;

    std::shared_ptr<Document> document;

    std::unique_ptr<Statistics> stats;

    std::unique_ptr<value> multi_match_ptr;

    void next();
  };

  MultiMatchGenerator(std::shared_ptr<QueryData> query_data, std::shared_ptr<Document> document);

  iterator begin() const;

  iterator end() const;

 private:
  std::shared_ptr<QueryData> query_data;

  std::shared_ptr<Document> document;
};

}  // namespace library_interface
}  // namespace REmatch