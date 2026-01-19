#pragma once

#include "REmatch/match_type_erased.hpp"

namespace REmatch {

class IteratorTypeErased {
 public:
  template <typename T,
            typename = std::enable_if_t<!std::is_same<std::decay_t<T>, IteratorTypeErased>::value>>
  explicit IteratorTypeErased(T&& it) : self(std::make_unique<Model<T>>(std::forward<T>(it))) {}

  IteratorTypeErased(IteratorTypeErased& other) = delete;
  IteratorTypeErased& operator=(const IteratorTypeErased&) = delete;

  IteratorTypeErased(IteratorTypeErased&&) noexcept = default;
  IteratorTypeErased& operator=(IteratorTypeErased&&) noexcept = default;

  IteratorTypeErased& operator++() {
    self->increment();
    return *this;
  }
  MatchTypeErased& operator*() const { return self->deref(); }

  MatchTypeErased* operator->() const { return self->arrow(); }

  bool operator==(const IteratorTypeErased& other) const { return self->equals(*other.self); }

  bool operator!=(const IteratorTypeErased& other) const { return !self->equals(*other.self); }

 private:
  struct Concept {
    virtual ~Concept() = default;
    virtual void increment() = 0;
    virtual MatchTypeErased& deref() const = 0;
    virtual MatchTypeErased* arrow() const = 0;
    virtual bool equals(const Concept&) const = 0;
  };

  template <typename T>
  struct Model final : Concept {
    explicit Model(T&& it) : object(std::forward<T>(it)) {}

    void increment() override { ++object; }

    MatchTypeErased& deref() const override { return *object; }

    MatchTypeErased* arrow() const override { return object.operator->(); }

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
