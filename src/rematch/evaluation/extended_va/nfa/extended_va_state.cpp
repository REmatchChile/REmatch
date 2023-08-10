#include "extended_va_state.hpp"

namespace rematch {

bool ExtendedVAState::is_initial() {
  return is_initial_;
}

bool ExtendedVAState::is_accepting() {
  return is_accepting_;
}

void ExtendedVAState::set_initial(bool is_initial) {
  is_initial_ = is_initial;
}

void ExtendedVAState::set_accepting(bool is_accepting) {
  is_accepting_ = is_accepting;
}

void ExtendedVAState::add_capture(std::bitset<64> code, ExtendedVAState* next) {
  for(auto const &capture: captures) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto *capture = new ExtendedVACapture(this, code, next);
  captures.push_back(capture);
  next->backward_captures.push_back(capture);
}

void ExtendedVAState::add_filter(CharClass charclass, ExtendedVAState* next) {
  for(auto const& filter: filters)
    if(filter->charclass == charclass && filter->next == next) return;

  auto *filter = new ExtendedVAFilter(this, charclass, next);
  filters.push_back(filter);
  next->backward_filters.push_back(filter);
}

void ExtendedVAState::add_read_capture(CharClass charclass,
                                        std::bitset<64> captures_set,
                                        ExtendedVAState* next) {
  for (auto const& read_capture : read_captures) {
    if (read_capture->charclass == charclass && read_capture->captures_set == captures_set)
      return;
  }

  auto* read_capture = new ExtendedVAReadCapture(this, charclass, captures_set, next);
  read_captures.push_back(read_capture);
  next->backward_read_captures.push_back(read_capture);
}

void ExtendedVAState::create_read_captures_forward() {
  for (auto& capture : captures) {
    for (auto& next_filter : capture->next->filters) {
      add_read_capture(next_filter->charclass, capture->code, next_filter->next);
    }

    if (capture->next->is_accepting()) {
      auto eof = CharClass(-1);
      add_read_capture(eof, capture->code, capture->next);
    }
  }
}

unsigned int ExtendedVAState::ID = 0;

}
