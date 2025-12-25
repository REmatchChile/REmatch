#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>

#include "REmatch/match_standard.hpp"
#include "REmatch_export.hpp"
#include "constants.hpp"
#include "match.hpp"

namespace REmatch {
class Document;
class Mediator;
struct QueryData;
struct Statistics;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {

class REMATCH_EXPORT MatchGenerator {
 public:
  struct REMATCH_EXPORT iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value = std::unique_ptr<Match>;
    using pointer = value*;
    using reference = value;

    // called with begin()
    explicit iterator(std::unique_ptr<Mediator> mediator_,
                      std::shared_ptr<VariableCatalog> variable_catalog_,
                      std::shared_ptr<Document> document_);

    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;

    // called with end()
    iterator();

    ~iterator();

    value operator*();

    iterator& operator++();
    void operator++(int);

    bool operator==(const iterator& other) const;

    bool operator!=(const iterator& other) const;

   private:
    std::unique_ptr<Mediator> mediator;

    std::shared_ptr<parsing::VariableCatalog> variable_catalog;

    std::shared_ptr<Document> document;

    std::unique_ptr<Statistics> stats;

    std::unique_ptr<Match> match_ptr;

    void next();
  };

  MatchGenerator(std::shared_ptr<QueryData> query_data, std::shared_ptr<Document> document);

  iterator begin() const;

  iterator end() const;

 private:
  std::shared_ptr<QueryData> query_data;
  std::shared_ptr<Document> document;
};

}  // namespace library_interface
}  // namespace REmatch