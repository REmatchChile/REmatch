#ifndef SEGMENT_MANAGER_HPP
#define SEGMENT_MANAGER_HPP

#include <memory>
#include "output_enumeration/span.hpp"

namespace rematch {

class SegmentManager {
 public:
  virtual std::unique_ptr<Span> next() = 0;
  virtual ~SegmentManager() = default;
};

}

#endif
