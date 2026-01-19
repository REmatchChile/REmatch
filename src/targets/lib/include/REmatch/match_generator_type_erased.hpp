#pragma once

#include <memory>
#include "REmatch/iterator_type_erased.hpp"

namespace REmatch {

class MatchGeneratorTypeErased {
 public:
  template <typename T, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<T>, MatchGeneratorTypeErased>::value>>
  explicit MatchGeneratorTypeErased(T&& obj)
      : self(std::make_unique<Model<T>>(std::forward<T>(obj))) {}

  MatchGeneratorTypeErased(MatchGeneratorTypeErased& other) = delete;
  MatchGeneratorTypeErased& operator=(const MatchGeneratorTypeErased&) = delete;

  MatchGeneratorTypeErased(MatchGeneratorTypeErased&&) noexcept = default;
  MatchGeneratorTypeErased& operator=(MatchGeneratorTypeErased&&) noexcept = default;

  IteratorTypeErased begin() const { return self->begin(); }
  IteratorTypeErased end() const { return self->end(); }

 private:
  struct Concept {
    virtual ~Concept();
    virtual IteratorTypeErased begin() const = 0;
    virtual IteratorTypeErased end() const = 0;
  };

  template <typename T>
  struct Model : public Concept {
    explicit Model(T&& obj) : object(std::forward<T>(obj)) {}

    IteratorTypeErased begin() const override {
      auto it = object.begin();
      return IteratorTypeErased(std::move(it));
    }
    IteratorTypeErased end() const override {
      auto it = object.end();
      return IteratorTypeErased(std::move(it));
    }

    T object;
  };

  std::unique_ptr<Concept> self;
};

}  // namespace REmatch
