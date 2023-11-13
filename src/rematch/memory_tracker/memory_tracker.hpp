#ifndef MEMORY_TRACKER_HPP
#define MEMORY_TRACKER_HPP

#include <cstddef>
#include "memory_limit_exceeded_exception.hpp"

static size_t DEFAULT_MEMORY_LIMIT = 1e8;

class MemoryTracker {
 private:
  // in bytes
  size_t memory_limit_ = DEFAULT_MEMORY_LIMIT;
  size_t memory_in_use_ = 0;

  MemoryTracker() {}
  MemoryTracker(MemoryTracker const&) = delete;
  MemoryTracker& operator=(MemoryTracker const&) = delete;

 public:
  static MemoryTracker& get_instance() {
    static MemoryTracker instance;
    return instance;
  }

  size_t get_memory_limit() const {
    return memory_limit_;
  }

  void set_memory_limit(size_t bytes) {
    memory_limit_ = bytes;
  };

  size_t get_memory_usage() const {
    return memory_in_use_;
  }

  void reset_memory_usage() {
    memory_in_use_ = 0;
  }

  void reset_memory_limit() {
    memory_limit_ = DEFAULT_MEMORY_LIMIT;
  }

  template <typename T>
  void track() {
    memory_in_use_ += sizeof(T);
    if (memory_in_use_ > memory_limit_) {
      throw REMatch::MemoryLimitExceededException();
    }
  }

  template <typename T>
  void untrack() {
    memory_in_use_ -= sizeof(T);
  }
};

#endif
