#pragma once

#include "REmatch/match.hpp"
#include "REmatch_export.hpp"

namespace REmatch {

// Forward iterator over the matches found in the document.
class REMATCH_EXPORT Iterator {
 public:
  template <typename T,
            typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Iterator>::value>>
  explicit Iterator(T&& it) : self(std::make_unique<Model<T>>(std::forward<T>(it))) {}

  Iterator(Iterator& other) = delete;
  Iterator& operator=(const Iterator&) = delete;

  Iterator(Iterator&&) noexcept = default;
  Iterator& operator=(Iterator&&) noexcept = default;

  Iterator& operator++() {
    self->increment();
    return *this;
  }
  Match& operator*() const { return self->deref(); }

  Match* operator->() const { return self->arrow(); }

  bool operator==(const Iterator& other) const { return self->equals(*other.self); }

  bool operator!=(const Iterator& other) const { return !self->equals(*other.self); }

 private:
  struct Concept {
    virtual ~Concept() = default;
    virtual void increment() = 0;
    virtual Match& deref() const = 0;
    virtual Match* arrow() const = 0;
    virtual bool equals(const Concept&) const = 0;
  };

  template <typename T>
  struct Model final : Concept {
    explicit Model(T&& it) : object(std::forward<T>(it)) {}

    void increment() override { ++object; }

    Match& deref() const override { return *object; }

    Match* arrow() const override { return object.operator->(); }

    bool equals(const Concept& other) const override {
      if (auto* p = dynamic_cast<const Model*>(&other))
        return object == p->object;
      return false;
    }

    T object;
  };

  std::unique_ptr<Concept> self;
};

}  // namespace REmatch
