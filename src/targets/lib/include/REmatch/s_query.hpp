#pragma once

#include <memory>
#include <string>
#include <vector>

#include "flags.hpp"
#include "fstream_reader.hpp"
#include "match.hpp"
#include "s_match_generator.hpp"

#include "REmatch_export.hpp"

namespace REmatch {

struct QueryData;

class REMATCH_EXPORT SQuery {
 public:
  explicit SQuery(const std::string& pattern, Flags flags,
                  uint_fast32_t max_mempool_duplications,
                  uint_fast32_t max_deterministic_states,
                  uint_fast32_t buffer_size);

  std::unique_ptr<SMatch> find(Reader* reader);

  SMatchGenerator finditer(Reader* reader);

  Flags flags_;
  std::shared_ptr<QueryData> query_data_;

  uint_fast32_t max_mempool_duplications;
  uint_fast32_t max_deterministic_states;
  uint_fast32_t buffer_size;
};
}  // namespace REmatch
