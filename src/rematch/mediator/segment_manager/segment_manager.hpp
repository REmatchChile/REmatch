#ifndef SEGMENT_MANAGER_HPP
#define SEGMENT_MANAGER_HPP

#include <memory>

#include <REmatch/span.hpp>

namespace REmatch {

class SegmentManager {
 public:
  virtual std::unique_ptr<Span> next() = 0;
  virtual ~SegmentManager() = default;
  virtual size_t get_search_dfa_size() = 0;
  virtual size_t get_search_nfa_size() = 0;
};

}

#endif
