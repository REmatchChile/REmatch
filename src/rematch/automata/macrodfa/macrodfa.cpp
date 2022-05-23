#include "macrodfa.hpp"

#include <memory>
#include <set>
#include <algorithm>

#include "automata/macrodfa/macrostate.hpp"
#include "automata/dfa/transition.hpp"

namespace rematch {

MacroDFA::MacroDFA(DFA& dA): dfa_(dA) {}

MacroState* MacroDFA::add_state(DState *state) {
  states_.push_back(new MacroState(state));
  return states_.back();
}

MacroState* MacroDFA::add_state(std::vector<DState*> states) {
  states_.push_back(new MacroState(states));
  return states_.back();
}

MacroTransition* MacroDFA::next_transition(MacroState* ms, char a) {

	// Set to store the key
	std::set<size_t> dstates_key;

	// Set to store the reached states
	std::set<DState*> dstates_storage;

	std::vector<std::pair<DState*, DState*>> first_storage, repeats_storage;

	int nfirstdirects = 0, nrepeatdirects = 0,
	    nfirstcaptures = 0, nrepeatcaptures = 0, nempties = 0;

	for(auto &state: ms->states()) {
		// Classic on-the-fly determinization
		auto nextTransition = state->next_transition(a);

		if(nextTransition == nullptr) {
			nextTransition = dfa_.next_transition(state, a);
		}

		if(nextTransition->type_ & Transition::kDirect) {
			auto res = dstates_key.insert(nextTransition->direct_->id());
			if (!res.second)
				nrepeatdirects++;
			else
				nfirstdirects++;
		} else if (nextTransition->type_ == Transition::kEmpty) {
			nempties++;
			continue;
		} if (nextTransition->type_ & Transition::kSingleCapture) {
			auto res = dstates_key.insert(nextTransition->capture_->next->id());
			if (!res.second)
				nrepeatcaptures++;
			else
				nfirstcaptures++;
		} else if(nextTransition->type_ & Transition::kMultiCapture) {
			for(auto &capture: nextTransition->captures_) {
				auto res = dstates_key.insert(capture->next->id());
				if (!res.second)
					nrepeatcaptures++;
				else
					nfirstcaptures++;
			}
		}
	}

	// Alloc a new MacroTransition
	std::shared_ptr<MacroTransition> mtrans = std::make_shared<MacroTransition>(nfirstdirects, nrepeatdirects,
																																						  nfirstcaptures, nrepeatcaptures, nempties);

	for(auto &state: ms->states()) {
		// Classic on-the-fly determinization
		auto nextTransition = state->next_transition(a);

		if(nextTransition->type_ & Transition::kDirect) {
			auto res = dstates_storage.insert(nextTransition->direct_);
			if (res.second)
				mtrans->add_direct(*state, *nextTransition->direct_, true);
			else
				mtrans->add_direct(*state, *nextTransition->direct_, false);
		} else if(nextTransition->type_ == Transition::kEmpty) {
			mtrans->add_empty(*state);
			continue;
		} if (nextTransition->type_ & Transition::kSingleCapture) {
			auto res = dstates_storage.insert(nextTransition->capture_->next);
			if (res.second)
				mtrans->add_capture(*state, nextTransition->capture_->S, *nextTransition->capture_->next, true);
			else
				mtrans->add_capture(*state, nextTransition->capture_->S, *nextTransition->capture_->next, false);
		} else if(nextTransition->type_ & Transition::kMultiCapture) {
			for(auto &capture: nextTransition->captures_) {
				auto res = dstates_storage.insert(capture->next);
				if (res.second)
					mtrans->add_capture(*state, capture->S, *capture->next, true);
				else
					mtrans->add_capture(*state, capture->S, *capture->next, false);
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
	if(found == mstates_table_.end()) {
		// Convert set to vector
		std::vector<DState*> real_dstates_storage(dstates_storage.begin(), dstates_storage.end());

		// Create the new MacroState
		q = add_state(real_dstates_storage);

		// Insert new MacroState in table
		mstates_table_.insert(std::pair<std::vector<size_t>, MacroState*>(real_dstates_key, q));
	} else {
		q = found->second;
	}

	mtrans->set_next_state(q);

	// mtrans->set_type();

	ms->add_transition(a, mtrans);

	return mtrans.get();
}

void MacroDFA::set_as_init(MacroState *ms) {init_state_ = ms;}

MacroState& MacroDFA::get_init_state() {
  return *init_state_;
}

} // end namespace rematch