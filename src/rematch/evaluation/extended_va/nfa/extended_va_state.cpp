#include "extended_va_state.hpp"

namespace REmatch {

ExtendedVAState::~ExtendedVAState() {
  for (auto& read_capture: read_captures) {
    delete read_capture;
  }
}

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
    if (read_capture->charclass == charclass &&
        read_capture->captures_set == captures_set &&
        read_capture->next == next)
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

void ExtendedVAState::delete_transitions() {
  for (auto &capture: backward_captures) {
    for (auto it=capture->from->captures.begin(); it != capture->from->captures.end();) {
      if (capture->from == (*it)->from && capture->next == (*it)->next)
        it = capture->from->captures.erase(it);
      else
        ++it;
    }
    delete capture;
  }

  for (auto &capture: captures) {
    for (auto it=capture->next->backward_captures.begin(); it != capture->next->backward_captures.end();) {
      if (capture->from == (*it)->from && capture->next == (*it)->next)
        it = capture->next->backward_captures.erase(it);
      else
        ++it;
    }
    delete capture;
  }

  for (auto &filter: backward_filters) {
    for (auto it=filter->from->filters.begin(); it != filter->from->filters.end();) {
      if (filter->from == (*it)->from && filter->next == (*it)->next)
        it = filter->from->filters.erase(it);
      else
        ++it;
    }
    delete filter;
  }

  for (auto &filter: filters) {
    for (auto it=filter->next->backward_filters.begin(); it != filter->next->backward_filters.end();) {
      if (filter->from == (*it)->from && filter->next == (*it)->next)
        it = filter->next->backward_filters.erase(it);
      else
        ++it;
    }
    delete filter;
  }

  for (auto& read_capture : backward_read_captures) {
    ExtendedVAState* previous_state = read_capture->from;
    for (auto it = previous_state->read_captures.begin(); it != previous_state->read_captures.end();) {
      if (previous_state == (*it)->from && read_capture->next == (*it)->next)
        it = previous_state->read_captures.erase(it);
      else
        ++it;
    }
    delete read_capture;
  }

  for (auto& read_capture : read_captures) {
    ExtendedVAState* next_state = read_capture->next;
    for (auto it = next_state->backward_read_captures.begin(); it != next_state->backward_read_captures.end();) {
      if (read_capture->from == (*it)->from && next_state == (*it)->next)
        it = read_capture->next->backward_read_captures.erase(it);
      else
        ++it;
    }
    delete read_capture;
  }

  backward_captures.clear();
  captures.clear();
  backward_captures.clear();
  filters.clear();
  backward_read_captures.clear();
  read_captures.clear();
}

unsigned int ExtendedVAState::ID = 0;

}
