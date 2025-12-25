#pragma once

#include <iterator>
#include <memory>
#include <vector>

#include "flags.hpp"
#include "fstream_reader.hpp"
#include "match.hpp"

#include "REmatch_export.hpp"

namespace REmatch {

class Document;
class Mediator;
class Stream;
struct QueryData;

inline namespace parsing {
class VariableCatalog;
}

inline namespace library_interface {
class SMatch;

class REMATCH_EXPORT SMatchGenerator {
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
                      std::shared_ptr<Stream> stream);

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
    std::shared_ptr<Stream> stream;

    std::unique_ptr<Match> match_ptr;

    void next();
  };

  SMatchGenerator(std::shared_ptr<QueryData> query_data, std::shared_ptr<Stream> stream);

  iterator begin() const;

  iterator end() const;

 private:
  std::shared_ptr<QueryData> query_data;
  std::shared_ptr<Stream> stream;
};

}  // namespace library_interface
}  // namespace REmatch
