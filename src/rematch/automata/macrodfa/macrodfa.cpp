#include "macrodfa.hpp"

#include <algorithm>
#include <memory>
#include <set>

#include "automata/dfa/transition.hpp"
#include "automata/macrodfa/macrostate.hpp"

namespace rematch {

MacroDFA::MacroDFA(DFA &dA) : dfa_(dA) {}

MacroState *MacroDFA::add_state(DFA::State *state) {
  states_.push_back(new MacroState(state));
  return states_.back();
}

MacroState *MacroDFA::add_state(std::vector<DFA::State *> states) {
  states_.push_back(new MacroState(states));
  return states_.back();
}

MacroTransition *MacroDFA::next_transition(MacroState *ms, char a) {

  // Set to store the key
  std::set<size_t> dstates_key;

  // Set to store the reached states
  std::set<DFA::State *> dstates_storage;

  std::vector<std::pair<DFA::State *, DFA::State *>> first_storage, repeats_storage;

  int nfirstdirects = 0, nrepeatdirects = 0, nfirstcaptures = 0,
      nrepeatcaptures = 0, nempties = 0;

  for (auto &state : ms->states()) {
    // Classic on-the-fly determinization
    #ifdef NOPT_ASCIIARRAY
    auto nextTransition = dfa_.next_base_transition(state, a);
    #else
    auto nextTransition = dfa_.next_transition(state, a);
    #endif

    DFA::State *direct = dynamic_cast<DFA::State*>(nextTransition.direct_),
               *cap    = dynamic_cast<DFA::State*>(nextTransition.capture_.next);

    if (nextTransition.type_ & Transition::kDirect) {
      auto res = dstates_key.insert(direct->id());
      if (!res.second)
        nrepeatdirects++;
      else
        nfirstdirects++;
    } else if (nextTransition.type_ == Transition::kEmpty) {
      nempties++;
      continue;
    }
    if (nextTransition.type_ & Transition::kSingleCapture) {
      auto res = dstates_key.insert(cap->id());
      if (!res.second)
        nrepeatcaptures++;
      else
        nfirstcaptures++;
    } else if (nextTransition.type_ & Transition::kMultiCapture) {
      for (auto &capture : nextTransition.captures_) {
        DFA::State *c = dynamic_cast<DFA::State*>(capture.next);
        auto res = dstates_key.insert(c->id());
        if (!res.second)
          nrepeatcaptures++;
        else
          nfirstcaptures++;
      }
    }
  }

  // Alloc a new MacroTransition
  std::shared_ptr<MacroTransition> mtrans = std::make_shared<MacroTransition>(
      nfirstdirects, nrepeatdirects, nfirstcaptures, nrepeatcaptures, nempties);

  for (auto &state : ms->states()) {
    // Classic on-the-fly determinization
    #ifdef NOPT_ASCIIARRAY
    auto nextTransition = dfa_.next_base_transition(state, a);
    #else
    auto nextTransition = dfa_.next_transition(state,a);
    #endif

    DFA::State *direct = dynamic_cast<DFA::State*>(nextTransition.direct_),
               *cap    = dynamic_cast<DFA::State*>(nextTransition.capture_.next);

    if (nextTransition.type_ & Transition::kDirect) {
      auto res = dstates_storage.insert(direct);
      if (res.second)
        mtrans->add_direct(*state, *direct, true);
      else
        mtrans->add_direct(*state, *direct, false);
    } else if (nextTransition.type_ == Transition::kEmpty) {
      mtrans->add_empty(*state);
      continue;
    }
    if (nextTransition.type_ & Transition::kSingleCapture) {
      auto res = dstates_storage.insert(cap);
      if (res.second)
        mtrans->add_capture(*state, nextTransition.capture_.S,
                            *cap, true);
      else
        mtrans->add_capture(*state, nextTransition.capture_.S,
                            *cap, false);
    } else if (nextTransition.type_ & Transition::kMultiCapture) {
      for (auto &capture : nextTransition.captures_) {
        DFA::State* c = dynamic_cast<DFA::State*>(capture.next);
        auto res = dstates_storage.insert(c);
        if (res.second)
          mtrans->add_capture(*state, capture.S, *c, true);
        else
          mtrans->add_capture(*state, capture.S, *c, false);
      }
    }
  }

  // Pass up to a vector
  std::vector<size_t> real_dstates_key(dstates_key.begin(), dstates_key.end());

  // Sorting needed to compute the correct key
  std::sort(real_dstates_key.begin(), real_dstates_key.end());

  auto found = mstates_table_.find(real_dstates_key);

  MacroState *q;

  // Check if not inside table already
  if (found == mstates_table_.end()) {
    // Convert set to vector
    std::vector<DFA::State *> real_dstates_storage(dstates_storage.begin(),
                                               dstates_storage.end());

    // Create the new MacroState
    q = add_state(real_dstates_storage);

    // Insert new MacroState in table
    mstates_table_.insert(
        std::pair<std::vector<size_t>, MacroState *>(real_dstates_key, q));
  } else {
    q = found->second;
  }

  mtrans->set_next_state(q);

  // mtrans->set_type();

  ms->add_transition(a, mtrans);

  return mtrans.get();
}

void MacroDFA::set_as_init(MacroState *ms) { init_state_ = ms; }

MacroState &MacroDFA::get_init_state() { return *init_state_; }

} // end namespace rematch