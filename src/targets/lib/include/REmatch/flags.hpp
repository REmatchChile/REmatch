#pragma once

#include <cstdint>

#include "REmatch_export.hpp"

namespace REmatch {
inline namespace library_interface {

enum class REMATCH_EXPORT Flags : uint8_t {
  NONE = 0U,
  LINE_BY_LINE = 1U << 0,
};

inline Flags operator|(Flags lhs, Flags rhs) {
  return static_cast<Flags>(static_cast<uint8_t>(lhs) |
                            static_cast<uint8_t>(rhs));
}

inline Flags& operator|=(Flags& lhs, Flags rhs) {
  return lhs = lhs | rhs;
}

inline Flags operator&(Flags lhs, Flags rhs) {
  return static_cast<Flags>(static_cast<uint8_t>(lhs) &
                            static_cast<uint8_t>(rhs));
}

inline Flags& operator&=(Flags& lhs, Flags rhs) {
  return lhs = lhs & rhs;
}

}  // namespace library_interface
}  // namespace REmatch
