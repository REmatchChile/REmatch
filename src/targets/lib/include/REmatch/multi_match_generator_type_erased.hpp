#pragma once

#include <memory>
#include "REmatch/multi_iterator_type_erased.hpp"

namespace REmatch {

class MultiMatchGeneratorTypeErased {
 public:
  template <typename T, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<T>, MultiMatchGeneratorTypeErased>::value>>
  explicit MultiMatchGeneratorTypeErased(T&& obj)
      : self(std::make_unique<Model<T>>(std::forward<T>(obj))) {}

  MultiMatchGeneratorTypeErased(MultiMatchGeneratorTypeErased& other) = delete;
  MultiMatchGeneratorTypeErased& operator=(const MultiMatchGeneratorTypeErased&) = delete;

  MultiMatchGeneratorTypeErased(MultiMatchGeneratorTypeErased&&) noexcept = default;
  MultiMatchGeneratorTypeErased& operator=(MultiMatchGeneratorTypeErased&&) noexcept = default;

  MultiIteratorTypeErased begin() const { return self->begin(); }
  MultiIteratorTypeErased end() const { return self->end(); }

 private:
  struct Concept {
    virtual ~Concept();
    virtual MultiIteratorTypeErased begin() const = 0;
    virtual MultiIteratorTypeErased end() const = 0;
  };

  template <typename T>
  struct Model : public Concept {
    explicit Model(T&& obj) : object(std::forward<T>(obj)) {}

    MultiIteratorTypeErased begin() const override {
      auto it = object.begin();
      return MultiIteratorTypeErased(std::move(it));
    }
    MultiIteratorTypeErased end() const override {
      auto it = object.end();
      return MultiIteratorTypeErased(std::move(it));
    }

    T object;
  };

  std::unique_ptr<Concept> self;
};

}  // namespace REmatch
