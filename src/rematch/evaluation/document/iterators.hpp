#ifndef EVALUATION__DOCUMENT__ITERATORS_HPP
#define EVALUATION__DOCUMENT__ITERATORS_HPP

#include <memory>

namespace rematch {

struct EndLineIterator {};

class LineIterator {
  public:
    LineIterator() = default;
    // Constructor: We can stuff anything into a Greeter costume.
    template <class T>
    LineIterator(T data) : self_(std::make_shared<Model<T>>(data)) {}

    // External interface: Just forward the call to the wrapped object.
    LineIterator& operator++() {
        self_->advance(); return *this;
    }

    const std::string& operator*() const {
      return **self_;
    }

		virtual bool operator!=(EndLineIterator e) const {
      return self_->at_end();
    }

  private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void advance() = 0;
        virtual const std::string& operator*() const = 0;
        virtual bool at_end() const = 0;
    };
    // ... and so are the templates.
    template <class T>
    class Model : public Concept {
      public:
        Model(T data) : data_(data) {}

        virtual void advance() override {
          ++data_;
        }

        virtual const std::string& operator*() const override {
          return *data_;
        }

        virtual bool at_end() const override {
          return data_.at_end();
        };


      private:
        T data_;
    };

    std::shared_ptr<Concept> self_;
};


struct EndCharIterator {};

class CharIterator {
  public:
    CharIterator() = default;
    // Constructor: We can stuff anything into a Greeter costume.
    template <class T>
    CharIterator(T &&data) : self_(std::make_shared<Model<T>>(std::move(data))) {}

    // External interface: Just forward the call to the wrapped object.
    CharIterator& operator++() {
      self_->advance(); return *this;
    }

    const char& operator*() const {
      return **self_;
    }

		virtual bool operator!=(EndCharIterator e) const {
      return self_->at_end();
    }

  private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void advance() = 0;
        virtual const char& operator*() const = 0;
        virtual bool at_end() const = 0;
    };
    // ... and so are the templates.
    template <class T>
    class Model : public Concept {
      public:
        Model(T &&data) : data_(std::move(data)) {}

        virtual void advance() override {
          ++data_;
        }

        virtual const char& operator*() const override {
          return *data_;
        }

        virtual bool at_end() const override {
          return data_.at_end();
        };


      private:
        T data_;
    };

    std::shared_ptr<Concept> self_;
};

} // end namespace rematch

#endif // EVALUATION__DOCUMENT__ITERATORS_HPP