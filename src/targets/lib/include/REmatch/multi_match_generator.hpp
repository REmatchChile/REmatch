#pragma once

#include <memory>
#include "REmatch/multi_iterator.hpp"

namespace REmatch {

class REMATCH_EXPORT MultiMatchGenerator {
 public:
  template <typename T, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<T>, MultiMatchGenerator>::value>>
  explicit MultiMatchGenerator(T&& obj)
      : self(std::make_unique<Model<T>>(std::forward<T>(obj))) {}

  MultiMatchGenerator(MultiMatchGenerator& other) = delete;
  MultiMatchGenerator& operator=(const MultiMatchGenerator&) = delete;

  MultiMatchGenerator(MultiMatchGenerator&&) noexcept = default;
  MultiMatchGenerator& operator=(MultiMatchGenerator&&) noexcept = default;

  MultiIterator begin() const { return self->begin(); }
  MultiIterator end() const { return self->end(); }

 private:
  struct Concept {
    virtual ~Concept();
    virtual MultiIterator begin() const = 0;
    virtual MultiIterator end() const = 0;
  };

  template <typename T>
  struct Model : public Concept {
    explicit Model(T&& obj) : object(std::forward<T>(obj)) {}

    MultiIterator begin() const override {
      auto it = object.begin();
      return MultiIterator(std::move(it));
    }
    MultiIterator end() const override {
      auto it = object.end();
      return MultiIterator(std::move(it));
    }

    T object;
  };

  std::unique_ptr<Concept> self;
};

}  // namespace REmatch
