#pragma once

#include "REmatch/match.hpp"
#include "REmatch/multi_match.hpp"

namespace REmatch {

// Forward iterator over the multi matches found in the document.
class REMATCH_EXPORT MultiIterator {
 public:
  template <typename T, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<T>, MultiIterator>::value>>
  explicit MultiIterator(T&& it)
      : self(std::make_unique<Model<T>>(std::forward<T>(it))) {}

  MultiIterator(MultiIterator& other) = delete;
  MultiIterator& operator=(const MultiIterator&) = delete;

  MultiIterator(MultiIterator&&) noexcept = default;
  MultiIterator& operator=(MultiIterator&&) noexcept = default;

  MultiIterator& operator++() {
    self->increment();
    return *this;
  }
  MultiMatch& operator*() const { return self->deref(); }

  MultiMatch* operator->() const { return self->arrow(); }

  bool operator==(const MultiIterator& other) const { return self->equals(*other.self); }

  bool operator!=(const MultiIterator& other) const { return !self->equals(*other.self); }

 private:
  struct Concept {
    virtual ~Concept() = default;
    virtual void increment() = 0;
    virtual MultiMatch& deref() const = 0;
    virtual MultiMatch* arrow() const = 0;
    virtual bool equals(const Concept&) const = 0;
  };

  template <typename T>
  struct Model final : Concept {
    explicit Model(T&& it) : object(std::forward<T>(it)) {}

    void increment() override { ++object; }

    MultiMatch& deref() const override { return *object; }

    MultiMatch* arrow() const override { return object.operator->(); }

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
