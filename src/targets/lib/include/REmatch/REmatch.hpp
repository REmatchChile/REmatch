// Main REmatch header
#pragma once

#include <cstdint>
#include <string>

#include "common.hpp"
#include "exceptions/exceptions.hpp"
#include "flags.hpp"
#include "match.hpp"
#include "match_iterator.hpp"
#include "multi_match.hpp"
#include "multi_match_iterator.hpp"
#include "multi_query.hpp"
#include "query.hpp"

namespace REmatch {

inline namespace library_interface {

Query reql(
    const std::string& pattern, Flags flags = Flags::NONE,
    uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    uint_fast32_t max_deterministic_states = DEFAULT_MAX_DETERMINISTIC_STATES);

MultiQuery multi_reql(
    const std::string& pattern, Flags flags = Flags::NONE,
    uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    uint_fast32_t max_deterministic_states = DEFAULT_MAX_DETERMINISTIC_STATES);
}  // namespace library_interface
}  // namespace REmatch
