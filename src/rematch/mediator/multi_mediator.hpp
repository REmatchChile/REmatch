#pragma once

#include <memory>
#include "output_enumeration/extended_mapping.hpp"

namespace REmatch {

class MultiMediator {
 public:
  virtual ~MultiMediator() = default;
  virtual std::unique_ptr<ExtendedMapping> next() = 0;
};
}  // namespace REmatch
