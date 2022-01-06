#include "sdfa.hpp"

#include <string>
#include <vector>

#include "det/setstate.hpp"

namespace rematch {

SearchDFA::SearchDFA(SearchVA const &A)
    : has_epsilon_(A.has_epsilon()),
      sVA_(A),
      vfactory_(A.variable_factory()),
      ffactory_(A.filter_factory()) {
  initial_state_ = new_dstate();
  initial_state_->add_state(A.initial_state());

  dstates_table_[initial_state_->bitmap()] = initial_state_;

}

DState* SearchDFA::new_dstate() {
  DState* np = new DState();
  states.push_back(np);
  return np;
}

Transition* SearchDFA::next_transition(DState *q, char a) {

	BitsetWrapper charBitset = ffactory_->applyFilters(a);

	std::set<State*> newSubset;  // Store the next subset
	BitsetWrapper subsetBitset(sVA_.size());  // Subset bitset representation

	for(auto &state: q->ss->subset) {
		for(auto &filter: state->filters) {
			if(charBitset.get(filter->code) && !subsetBitset.get(filter->next->id)) {
				newSubset.insert(filter->next);
				subsetBitset.set(filter->next->id, true);
			}
		}
	}

	auto found = dstates_table_.find(subsetBitset);

	DetState* nq;

	if(found == dstates_table_.end()) { // Check if already stored subset
		SetState* ss = new SetState(eVA_, newSubset);
		nq = new DetState(ss);

		dstates_table_[ss->bitstring] = nq;

		states.push_back(nq);

		if(nq->isFinal) {
			finalStates.push_back(nq);
		}
	} else {
		nq = found->second;
	}
	if(nq->ss->isNonEmpty) {
		computeCaptures(q, nq, a);
		// for(auto &state: nq->ss->subset){
			// if(!state->filters.empty() || state->isFinal){
				q->add_direct(a, nq);
				// break;
			// }
		// }
	} else {
		q->add_empty(a, nq);
	}

	return q->next_transition(a);
}

void DFA::computeCaptures(DetState* p, DetState* q, char a) {
	/* Computes the reachable subsets from the deterministic state q through
	captures, stores them if necessary and connects q to the computed
	deterministic states thourgh deterministic capture transitions */

	std::unordered_map<std::bitset<32>, std::set<State*>> captureList;
	std::unordered_map<std::bitset<32>, std::set<State*>>::iterator it;

	for(auto &extState: q->ss->subset) {
		for(auto &capture: extState->captures) {

			it = captureList.find(capture->code);

			if(it == captureList.end()) {
				it = captureList.emplace(capture->code, std::set<State*>()).first;
			}
			it->second.insert(capture->next);
		}
	}

	for(std::pair<std::bitset<32>, std::set<State*>> el: captureList){
		SetState* nss = new SetState(eVA_, el.second);

		/* Check if subset is not on hash table */

		auto found = dstates_table_.find(nss->bitstring);

		if(found == dstates_table_.end()) {
			DetState* nq = new DetState(nss);
			dstates_table_[nss->bitstring] = nq;

			states.push_back(nq);

			if (nq->isFinal) {
				finalStates.push_back(nq);
			}
		}

		DetState* r = dstates_table_[nss->bitstring]; // This is the deterministic state where the capture reaches

		p->add_capture(a, el.first, r);
	}
}


std::string DFA::pprint() {
  /* Gives a codification for the automaton that can be used to visualize it
     at https://puc-iic2223.github.io . It uses Breath-First Search
     to get every labeled transition in the automaton with the unique ids for
     each state */

  // Declarations
  std::stringstream ss, cond;
  DetState *current;
  std::bitset<32> S;

  // Typical set construction for keeping visited states
  std::unordered_set<unsigned int> visited;

  // Use of list to implement a FIFO queue
  std::list<DetState*> queue;

  // Start on the init state
  visited.insert(init_state_->id);
  queue.push_back(init_state_);

  // Start BFS
  while(!queue.empty()) {
    current = queue.front();
    queue.pop_front();

    for (size_t i = 0; i < 128; i++) {
      if(current->transitions_[i] == nullptr) continue;
      auto trans = current->transitions_[i].get();
      cond.clear();
      cond << "t " << *current << ' ' ;
      if(i == 0)
          ss << "0x0";
        else if((char)i == '\n')
          ss << "\\n";
        else if((char)i == ' ')
          ss << "' '";
        else if((char)i == '\t')
          ss << "\\t";
        else
          ss << (char)i;
      std::string prefix = cond.str();
      if(trans->type_ & Transition::Type::kDirect) {// Direct type
        ss << prefix << ' ' << trans->direct_->id;
        if (visited.find(trans->direct_->id) == visited.end()) {
          visited.insert(trans->direct_->id);
          queue.push_back(trans->direct_);
        }
      }
      if(trans->type_ & Transition::Type::kSingleCapture) {
        ss << prefix << '/' << variable_factory_->print_varset(trans->capture_->S)
                     << ' ' << trans->capture_->next->id;
        if (visited.find(trans->capture_->next->id) == visited.end()) {
          visited.insert(trans->capture_->next->id);
          queue.push_back(trans->capture_->next);
        }
      } // Single capture
      if(trans->type_ & Transition::Type::kMultiCapture) {
        for(auto &capture: trans->captures_) {
          ss << prefix << '/' << variable_factory_->print_varset(capture->S)
                       << ' ' << capture->next->id;
          if (visited.find(capture->next->id) == visited.end()) {
            visited.insert(capture->next->id);
            queue.push_back(capture->next);
          }
        }
      }
    }
  }

  // Code final states
  for (size_t i = 0; i < finalStates.size(); ++i) {
    if(finalStates[i]->isFinal) {
      ss << "f " << *finalStates[i] << '\n';
    }
  }

  // Code initial state
  ss << "i " << *init_state_;

  return ss.str();
}

} // end namespace rematch
