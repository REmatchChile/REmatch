#pragma once

#include <iterator>
#include <memory>
#include <vector>

#include "REmatch/match.hpp"
#include "evaluation/stream.hpp"
#include "mediator/mediator.hpp"

namespace REmatch::internal {

class SMatchGenerator {
 public:
  struct iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value = REmatch::Match;
    using pointer = value*;
    using reference = value&;

    // called with begin()
    explicit iterator(std::unique_ptr<Mediator> mediator_,
                      std::shared_ptr<VariableCatalog> variable_catalog_,
                      std::shared_ptr<Stream> stream);

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
    std::unique_ptr<Mediator> mediator;

    std::shared_ptr<parsing::VariableCatalog> variable_catalog;
    std::shared_ptr<Stream> stream;

    std::unique_ptr<REmatch::Match> match_ptr;

    void next();
  };

  SMatchGenerator(std::shared_ptr<QueryData> query_data, std::shared_ptr<Stream> stream);

  iterator begin() const;

  iterator end() const;

 private:
  std::shared_ptr<QueryData> query_data;
  std::shared_ptr<Stream> stream;
};

}  // namespace REmatch::internal
