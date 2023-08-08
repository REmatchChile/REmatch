
#include "extended_va_state.hpp"
#include "extended_va_capture.hpp"
#include "extended_va_filter.hpp"

namespace rematch {

void ExtendedVAState::add_capture(std::bitset<64> code, ExtendedVAState* next) {
  for(auto const &capture: captures) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto *capture = new ExtendedVACapture(this, code, next);
  captures.push_back(capture);
  next->backward_captures.push_back(capture);
}

void ExtendedVAState::add_filter(rematch::parsing::CharClass charclass, ExtendedVAState* next) {
  for(auto const& filter: filters)
    if(filter->charclass == charclass && filter->next == next) return;

  auto *filter = new ExtendedVAFilter(this, charclass, next);
  filters.push_back(filter);
  next->backward_filters.push_back(filter);
}
}
