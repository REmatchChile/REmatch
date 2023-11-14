#ifndef MEMORY_TRACKER_HPP
#define MEMORY_TRACKER_HPP

#include <cstddef>
#include <vector>
#include <unordered_map>
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
  void track(const std::vector<T>& vector) {
    memory_in_use_ += sizeof(T) * vector.size();
    throw_exception_if_memory_limit_is_exceeded();
  }

  template <typename K, typename V>
  void track(const std::unordered_map<K, V>& map) {
    memory_in_use_ += (sizeof(K) + sizeof(V)) * map.size();
    throw_exception_if_memory_limit_is_exceeded();
  }

  template <typename T>
  void track(const T& object) {
    memory_in_use_ += sizeof(T);
    throw_exception_if_memory_limit_is_exceeded();
  }

  template <typename K, typename V>
  void untrack(const std::unordered_map<K, V>& map) {
    memory_in_use_ -= (sizeof(K) + sizeof(V)) * map.size();
  }

  template <typename T>
  void untrack(const std::vector<T>& vector) {
    memory_in_use_ -= sizeof(T) * vector.size();
  }

  template <typename T>
  void untrack(const T& object) {
    memory_in_use_ -= sizeof(T);
  }

  void throw_exception_if_memory_limit_is_exceeded() {
    if (memory_in_use_ > memory_limit_) {
      throw REMatch::MemoryLimitExceededException();
    }
  }
};

#endif
