#pragma once

#include <memory>

#include "REmatch/iterator.hpp"
#include "REmatch_export.hpp"

namespace REmatch {

class REMATCH_EXPORT MatchGenerator {
 public:
  template <typename T,
            typename = std::enable_if_t<!std::is_same<std::decay_t<T>, MatchGenerator>::value>>
  explicit MatchGenerator(T&& obj) : self(std::make_unique<Model<T>>(std::forward<T>(obj))) {}

  MatchGenerator(MatchGenerator& other) = delete;
  MatchGenerator& operator=(const MatchGenerator&) = delete;

  MatchGenerator(MatchGenerator&&) noexcept = default;
  MatchGenerator& operator=(MatchGenerator&&) noexcept = default;

  Iterator begin() const { return self->begin(); }
  Iterator end() const { return self->end(); }

 private:
  struct Concept {
    virtual ~Concept();
    virtual Iterator begin() const = 0;
    virtual Iterator end() const = 0;
  };

  template <typename T>
  struct Model : public Concept {
    explicit Model(T&& obj) : object(std::forward<T>(obj)) {}

    Iterator begin() const override {
      auto it = object.begin();
      return Iterator(std::move(it));
    }
    Iterator end() const override {
      auto it = object.end();
      return Iterator(std::move(it));
    }

    T object;
  };

  std::unique_ptr<Concept> self;
};

}  // namespace REmatch
