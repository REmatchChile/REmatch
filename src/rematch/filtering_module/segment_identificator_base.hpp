#pragma once

#include <memory>

#include "REmatch/span.hpp"

namespace REmatch {
inline namespace filtering_module {

class SegmentIdentificatorBase {
 public:
  SegmentIdentificatorBase() = default;
  virtual ~SegmentIdentificatorBase() = default;

  virtual std::unique_ptr<Span> next() = 0;
  virtual void set_document_indexes(Span& span) = 0;
  virtual size_t get_search_dfa_size() const = 0;
  virtual size_t get_search_nfa_size() const = 0;
  virtual void evaluate_start_char() = 0;
};

}  // namespace filtering_module
}  // namespace REmatch
