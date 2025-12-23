// Main REmatch header
#pragma once

#include <cstdint>
#include <string>

#include "REmatch_export.hpp"

#include "constants.hpp"
#include "exceptions.hpp"
#include "flags.hpp"
#include "fstream_reader.hpp"
#include "match.hpp"
#include "match_generator.hpp"
#include "multi_match.hpp"
#include "multi_match_generator.hpp"
#include "multi_query.hpp"
#include "query.hpp"
#include "s_match.hpp"
#include "s_match_generator.hpp"
#include "s_query.hpp"
#include "span.hpp"

namespace REmatch {

inline namespace library_interface {

REMATCH_EXPORT Query
reql(const std::string& pattern, Flags flags = Flags::NONE,
     uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
     uint_fast32_t max_deterministic_states = DEFAULT_MAX_DETERMINISTIC_STATES);

REMATCH_EXPORT SQuery reql_stream(
    const std::string& pattern, Flags flags = Flags::NONE,
    uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    uint_fast32_t max_deterministic_states = DEFAULT_MAX_DETERMINISTIC_STATES,
    uint_fast32_t buffer_size = DEFAULT_STREAM_BUFFER_SIZE);

REMATCH_EXPORT MultiQuery multi_reql(
    const std::string& pattern, Flags flags = Flags::NONE,
    uint_fast32_t max_mempool_duplications = DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
    uint_fast32_t max_deterministic_states = DEFAULT_MAX_DETERMINISTIC_STATES);
}  // namespace library_interface
}  // namespace REmatch
