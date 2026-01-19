#pragma once

#include "REmatch/match_type_erased.hpp"
#include "REmatch/multi_match_type_erased.hpp"

namespace REmatch {

class MultiIteratorTypeErased {
 public:
  template <typename T, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<T>, MultiIteratorTypeErased>::value>>
  explicit MultiIteratorTypeErased(T&& it)
      : self(std::make_unique<Model<T>>(std::forward<T>(it))) {}

  MultiIteratorTypeErased(MultiIteratorTypeErased& other) = delete;
  MultiIteratorTypeErased& operator=(const MultiIteratorTypeErased&) = delete;

  MultiIteratorTypeErased(MultiIteratorTypeErased&&) noexcept = default;
  MultiIteratorTypeErased& operator=(MultiIteratorTypeErased&&) noexcept = default;

  MultiIteratorTypeErased& operator++() {
    self->increment();
    return *this;
  }
  MultiMatchTypeErased& operator*() const { return self->deref(); }

  MultiMatchTypeErased* operator->() const { return self->arrow(); }

  bool operator==(const MultiIteratorTypeErased& other) const { return self->equals(*other.self); }

  bool operator!=(const MultiIteratorTypeErased& other) const { return !self->equals(*other.self); }

 private:
  struct Concept {
    virtual ~Concept() = default;
    virtual void increment() = 0;
    virtual MultiMatchTypeErased& deref() const = 0;
    virtual MultiMatchTypeErased* arrow() const = 0;
    virtual bool equals(const Concept&) const = 0;
  };

  template <typename T>
  struct Model final : Concept {
    explicit Model(T&& it) : object(std::forward<T>(it)) {}

    void increment() override { ++object; }

    MultiMatchTypeErased& deref() const override { return *object; }

    MultiMatchTypeErased* arrow() const override { return object.operator->(); }

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
